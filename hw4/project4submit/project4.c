#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<fcntl.h> 
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#define COMP_PENALTY -100
#define ERROR_PENALTY -10
#define MEM_ACCESS_PENALTY -15

typedef struct score{
    int compilation;
    int output;
    int mem_access;
    int total;
}scoreT;

//seperates the progname using the <progname>.c as input
char *find_progname(char* str);
//returns the name of the file (<progname>_err) the compiler messages are written 
char *find_progname_err(char* str);
//creates the first process to compile the program
int compilation_process (char *progname_err, char *progname, char *progname_c);
//calculate the size of a file
int file_size(int fd);
//executes the <progname> using processes and returns the percentage of similarity with the expected output
scoreT* output_process(char *progname, char *progargs, char *progin, char *progout, scoreT* score_ptr);
//executes the prog using the file with the arguments
int exec_args(char *progname, char* progargs);
//returns the score p4diff program returned, using the exit status
int output_score(int status);
//if the child process returned with SIGSEGV, SIGBUS or SIGABRT there is a memory access penalty returned
int memory_access_score(int status);
//print the the total score
void print_score(int compilation, int output, int memory);
//has the same functionality as system funcition read, but also checks the return value and reads in repetition when needed 
int my_read(int fd, char* buffer, int size);

int main(int argc,char* argv[]){
    char *str, *progname;
    scoreT stud_score;
    scoreT* stud_score_ptr;

    if(argc != 5){
        fprintf(stderr,"\nInvalid arguments.\n");
        return (2);
    }
    //struct init
    stud_score.compilation = 0;
    stud_score.mem_access = 0;
    stud_score.output = 0;
    stud_score.total = 0;
    stud_score_ptr = &stud_score;

    progname = find_progname(argv[1]);
    str = find_progname_err(argv[4]);
    stud_score_ptr->compilation = compilation_process(str, progname, argv[1]);
    //if the compilation was successful ( with or without warnings )
    if(stud_score_ptr->compilation != COMP_PENALTY){
       stud_score_ptr = output_process(progname, argv[2], argv[3], argv[4], stud_score_ptr);
    }
    print_score( stud_score_ptr->compilation,stud_score_ptr->output,stud_score_ptr->mem_access);

    free(str);
    free(progname);
    return 0;
}
void print_score(int compilation, int output, int memory){
    int total;

    printf("Compilation: %d\nOutput: %d\nMemory access: %d\n",compilation, output, memory);
    total = compilation + output + memory;
    if(total < 0){
        total = 0;
    }
    printf("Total: %d\n", total);


}
char *find_progname(char* str){
    int len;
    char* progname;

    len = strlen(str);
    //the str is <progname>.c, the .c is removed 
    strtok(str, ".");
    //the length returned excludes \0
    progname = (char*) malloc(sizeof(char) * (len-1));
    strcpy(progname, str);
    progname[len-2] = '\0';
    //add the ending .c since strtok seperated the str 
    str[len-2] = '.';
    str[len-1] = 'c';
    return(progname);
}

char *find_progname_err(char* str){
    int len;
    char *progname_err;

    //len of <progname>.out
    len = strlen(str);
    progname_err = (char*) malloc(sizeof(char) * (len+1));
    strcpy(progname_err, str);
    //use '\0' to separate the "out" string 
    progname_err[len-3] = '\0';
    //use strcat to append the string and add the "err"
    strcat(progname_err,"err");
    progname_err[len] = '\0';
   
    return (progname_err);   
}

int compilation_process (char *progname_err, char *progname, char *progname_c){ 
    int pid, fd_child, fd_parent, status, size;
    char *buffer;

    pid = fork();
    if( pid == -1){
        perror("Error in creating the first process.");
        exit(EXIT_FAILURE);
    }
    //code for child 
    else if( !pid){
        //open or creat the file to be used for the errors
        fd_child = open(progname_err, O_RDWR|O_CREAT|O_TRUNC, 0600);
        //redirect the strerr to the file created
        dup2(fd_child, STDERR_FILENO);
        execl("/usr/bin/gcc","gcc", "-Wall", progname_c, "-o", progname, NULL);
        perror("execl");
        close(fd_child);
        exit(EXIT_FAILURE);
    }
   
    //code for parent
    //wait until the child process ends
    waitpid( pid, &status, 0);
    fd_parent = open(progname_err, O_RDONLY, S_IRWXU);
    if(fd_parent == -1){
        perror("error opening the error file in parent process");
        exit(EXIT_FAILURE);
    }
    size = file_size(fd_parent);
    //if the file size is 0 the compilation was successful
    if(size != 0){
        buffer = (char*) malloc( (size+1) * sizeof(char));
        if(buffer == NULL){
            perror("Error in malloc in compilation_proccess function");
            exit(EXIT_FAILURE);
        }
        buffer[0] = '\0';
        lseek(fd_parent, 0L, SEEK_SET);
        my_read(fd_parent, buffer, size);
        buffer[size] = '\0';

        if( strstr(buffer, "error:") != NULL){
            free(buffer);
            close(fd_parent);
            return(COMP_PENALTY);
        }
        else if( strstr(buffer, "warning:") != NULL){
            free(buffer);
            close(fd_parent);
            return(ERROR_PENALTY);
        }
        
        free(buffer);
    }
    close(fd_parent);
    return (0);
}

int file_size(int fd){
    int size,res_read;
    char ch;

    size = 0;
    lseek(fd, 0L, SEEK_SET);
    while(1){
        res_read = read(fd, &ch, 1);
        if(res_read == 0){
            break;
        }
        size++;
    }
    return(size);
}

scoreT* output_process(char *progname, char *progargs, char *progin, char *progout, scoreT* score_ptr){
    int pipe_fd[2], pid_exec_args, in_fd, pid_exec_diff, check_exec, status_diff, status_args;
   
    //create an anonymus pipe
    pipe(pipe_fd);

    //code for the process that executes the program with the given arguments
    if( !(pid_exec_args = fork()) ){
       
        //close the read end of the pipe, since the process reads from the file 
        close(pipe_fd[0]);
        //open the input file
        in_fd = open(progin, O_RDONLY, S_IRWXU);
        if( in_fd == -1){
            perror("Error in opening the <progname>.in");
            exit(EXIT_FAILURE);
        }
        //redirect the std_in of the process to the input file
        dup2(in_fd, STDIN_FILENO);
        //redirect the std_out of the process to the write end of the pipe
        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[1]);
        exec_args(progname, progargs);

        return(0);
    }
    //code for the process that executes p4diff program
    else if(! (pid_exec_diff = fork()) ){
        //redirect the std_in to the read end of pipe
        dup2(pipe_fd[0], STDIN_FILENO);
        //close the write end of the pipe
        close(pipe_fd[1]);
        close(pipe_fd[0]);
        check_exec = execl( "p4diff", "p4diff", progout, NULL);
        if(check_exec == -1){
            perror("Error in executing p4diff");
        }
        return 0;
    }
    else{
        close(pipe_fd[1]);
        close(pipe_fd[0]);
        //wait until the child process ends
        waitpid(pid_exec_diff, &status_diff, 0);
        waitpid(pid_exec_args, &status_args, 0);
        score_ptr->mem_access = memory_access_score(status_args);
        score_ptr->output = output_score(status_diff);
    }
    return(score_ptr);
}
int output_score(int status){
    int score;

    //the proccess terminated normally without signal
    if( WIFEXITED(status)){
        //acquire the program's exit status 
        score = WEXITSTATUS(status);
    }
    return(score);
}

int memory_access_score(int status){
    int score = 0;
    
    //the proccess terminated normally without signal
    if(WIFEXITED(status)){
        return(0);
    }
    //the child process terminated with signal
    else if( WIFSIGNALED(status) ){
        //check which signal was returned
        if( (WTERMSIG(status) == SIGSEGV) || (WTERMSIG(status) == SIGABRT) || (WTERMSIG(status) == SIGBUS) ){
            score = MEM_ACCESS_PENALTY;
        }
    }
    return(score);
}

int exec_args(char *progname, char* progargs){
    int fd, size, args_num, i, j, exec_check;
    char *buffer, **argv_prog;

    //open the argument file
    fd = open(progargs, O_RDONLY);
    if(fd == -1){
        perror("Error in opening the argument file");
        exit(EXIT_FAILURE);
    }
    size = file_size(fd);
   
    //the argument's file is not empty 
    if(size != 0){   
        buffer = (char*) malloc(sizeof(char) * size);
        buffer[0] = '\0';
        //move the file descriptor in the begining of the file
        lseek(fd, 0L, SEEK_SET);
        my_read(fd, buffer, size);
        
        //first word since the argument file is not empty
        args_num = 1;
        for(i=0; i < size; i++){
            if( (buffer[i] == ' ') || (buffer[i] == '\n') ){
                buffer[i] = '\0';
                if( i != (size-1) ){
                    args_num++;
                }
                //ignore the multiple white characters in the end
                if( (i != (size -1)) && (buffer[i+1] == ' ') ){
                    args_num--;
                    break;
                }
            }
        }
        //the total number of arguments is args_num the pointer array has 2 extra positions:pos 0 for progname, the last for NULL 
        argv_prog = (char**) malloc(sizeof(char*) * (args_num+2) );
        if(argv_prog == NULL){
            perror("Malloc failed");
            exit(EXIT_FAILURE);
        }
        //init
        for(i=0; i<(args_num + 2); i++){
            argv_prog[i] = NULL;
        }
        //points at progname
        argv_prog[0] = progname;
        //pos 1 points at the first word
        argv_prog[1] = &buffer[0];
        j=2;
        for(i=0; i < size; i++){
            //if there are continuous white spaces in the end of the arguments
            if( (i != (size-1)) && (buffer[i] == '\0') && (buffer[i+1] == '\0') ){
                break;
            }
            else if( buffer[i] == '\0'){
                argv_prog[j] = &buffer[i + 1];
                j++;
            }
        }
        //the argv array must end with NULL
        argv_prog[args_num + 1] = NULL;
    }

    //the program takes no arguments
    else{
        args_num = 0;
        argv_prog = (char**) malloc(2*sizeof(char*));
        argv_prog[0] = progname;
        argv_prog[1] = NULL;
    }
    exec_check = execv(progname, argv_prog);
    if( exec_check == -1){
        perror("Error in execution of <progname>");
        exit(EXIT_FAILURE);
    }
    return(0); 
}

int my_read(int fd, char* buffer, int size){
    int res_read, temp;

    res_read = read(fd, buffer, size);
    //read failed
    if(res_read == -1){
        perror("Error in reading");
        exit(EXIT_FAILURE);
    }
    //reached EOF
    else if(res_read == 0){
        return(res_read);
    }
    //init to 1, so as to read at least one more time
    temp = 1;
    while(res_read != 0){
        if( (res_read < size) && (temp != 0)){
            temp = read(fd, &buffer[res_read], (size - res_read) );
            res_read += temp;
        }
        else{
            break;
        }
    }
    return(res_read);
}