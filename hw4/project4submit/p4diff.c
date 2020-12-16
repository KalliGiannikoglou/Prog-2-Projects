#include<stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include<string.h>
#define BUFFER_SIZE 65

typedef struct bytes{
    int golden_sample_bytes;
    int myout_bytes;
    int similar_bytes;

}bytesT;

//function that reads from stdin and compares the bytes read with the bytes of the given file. returns the number of bytes 
//read from stdin and the number of similar bytes
bytesT* compare_bytes(int fd_golden, bytesT* ptr_bytes);
//function that calculates the size of a given file
int file_size(int fd);
//calculates the percentage of similarity
int similarity(int golden_sample_size, int myout_size, int similar_bytes);

int main(int argc, char* argv[]){
    int fd_golden, percentage;
    bytesT read_bytes;
    bytesT *ptr_bytes;

    if(argc != 2){
        fprintf(stderr,"Invalid arguments.\n");
        return 0;
    }
    
    ptr_bytes = &read_bytes;
    //init the struct
    read_bytes.golden_sample_bytes = 0;
    read_bytes.myout_bytes = 0;
    read_bytes.similar_bytes = 0;

    //open the file given as arguments
    fd_golden = open(argv[1], O_RDONLY);
    if(fd_golden == -1){
        perror("Error in opening the file");
        exit(EXIT_FAILURE);
    }
    //find the percentage of similarity between the golden sample and the input
    ptr_bytes = compare_bytes(fd_golden, ptr_bytes);
    //calculate the size of the golden sample
    ptr_bytes->golden_sample_bytes = file_size(fd_golden);
    //calculare the final percentage
    percentage = similarity(ptr_bytes->golden_sample_bytes, ptr_bytes->myout_bytes, ptr_bytes->similar_bytes);
    
    close(fd_golden);
    return (percentage);
}

bytesT* compare_bytes(int fd_golden, bytesT* ptr_bytes){
    char golden_buff[BUFFER_SIZE], myprog_buff[BUFFER_SIZE];
    int res_read_gold, res_read_myprog, offset, gold_buf_size, myprog_buf_size, iteration_lim, temp;

    while(1){
        //init 
        golden_buff[0] = '\0';
        memset(myprog_buff, '\0', BUFFER_SIZE);
        myprog_buf_size = 0;
        gold_buf_size = 0;
        iteration_lim =0;
 
        //read the first 64 bytes from the golden sample
        res_read_gold = read(fd_golden, golden_buff, BUFFER_SIZE-1);
        if(res_read_gold == -1){
            perror("Error in reading from golden file");
            exit(EXIT_FAILURE);
        }
        //in order to use string.h functions correctly
        golden_buff[res_read_gold] = '\0';
        gold_buf_size = strlen(golden_buff);

        //read the first 64 bytes from the stdin
        res_read_myprog = read(STDIN_FILENO, myprog_buff, BUFFER_SIZE-1);
        if(res_read_myprog == -1){
            perror("Error in reading from stdin");
            exit(EXIT_FAILURE);
        }
        temp = 1;
        //reads the remaining bytes if there are any and are less than the buffer size (64)
        while(res_read_myprog != 0){
            if( (res_read_myprog < BUFFER_SIZE-1) && (temp != 0)){
                temp = read(STDIN_FILENO, &myprog_buff[res_read_myprog], (BUFFER_SIZE-1-res_read_myprog) );
                res_read_myprog += temp;
            }
            else{
                break;
            }
        }
        //end of data from stdin AND reached EOF 
        if( (res_read_myprog==0) && (res_read_gold == 0) ) {
            break;
        }
        //total size of data from stdin
        ptr_bytes->myout_bytes += strlen(myprog_buff);
        //size of the current iteration
        myprog_buf_size = strlen(myprog_buff); 

        //find which buffer is longer
        //if one of the two buffers is empty, the two bytes are not similar
        if( (gold_buf_size == 0) || (myprog_buf_size == 0)){
            continue;
        } 
        //the limit is set according to the shorter string
        else if( gold_buf_size >= myprog_buf_size){
            iteration_lim = myprog_buf_size;
        }
        else{
            iteration_lim = gold_buf_size;
        }
        //compare byte by byte the two buffers
        for(offset=0; offset<iteration_lim; offset++){
            if(golden_buff[offset] == myprog_buff[offset]){      
                ptr_bytes->similar_bytes++;
            }
        }
    }
    return (ptr_bytes);
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
int similarity(int golden_sample_size, int myout_size, int similar_bytes){
    int percentage, max;
    
    if(golden_sample_size >= myout_size){
        max = golden_sample_size;
    }
    else{
        max = myout_size;
    }
    //empty file case
    if(max == 0){
        return(100);
    }
    percentage = similar_bytes * 100 / max; 

    return(percentage);
}