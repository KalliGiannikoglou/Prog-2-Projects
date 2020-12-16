#include "objdb.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>


void openWrapper(curr_dbT* curr_db){
    int res_o;
    char dbname[256];

    //initialize dbname string 
    dbname[0] = '\0';
    scanf(" %s", dbname);
    strcpy(curr_db->dbname, dbname);

    res_o = openFile( dbname, curr_db);           
    if(res_o == -1){
        printf("\nInvalid db file %s.\n", dbname);
    }
    else if(res_o == 0){
        printf("\nError opening %s.\n", dbname);
    }
}

int importWrapper(curr_dbT* curr_db){
    int res_open, res_imp, res_find;
    char filename[256], objname[256];

    //initialize the strings 
    objname[0] = '\0';
    filename[0] = '\0';

    //check if a database is open
    res_open = openDatabase(curr_db);
    if(res_open == 0){
        printf("\nNo open db file.\n");
        return (1);
    }
    
    scanf(" %s %s", filename, objname);
    //through findWrapper, check if the obname read already exists in the database
    res_find = findWrapper(curr_db, objname, 0);
    if(res_find == 1){
        printf("\nObject %s already in db.\n", objname);
        return(1);    
    }
    //import an object file in the database
    res_imp = importFile( curr_db->file , filename, objname);
    if(res_imp == 0){
        printf("\nFile %s not found.\n", filename);
        return(1);
    }
    return 0;
}

int findWrapper(curr_dbT* curr_db,  char objname[256], int find_flag){
    int res_open, res_search;

    res_open = openDatabase(curr_db);
    if(res_open == 0){
        printf("\nNo open db file.\n");
        return(1);
    }

    //this option is true when find is called by other functions and not by the user
    if(find_flag == 0){
        res_search = searchObjname(curr_db->file, objname);
        return(res_search);
    }
    else{
        scanf(" %s", objname);
        if(strcmp( "*", objname) == 0){
            printAll( curr_db->file);
        }
        else{
            findStr(curr_db->file, objname);
        }
    }

    return 0;
}

int exportWrapper(curr_dbT* curr_db){
    int res_open, res_search;
    FILE *file_pointer;
    char objname[256], fname[256];

    //init
    objname[0] = '\0';
    fname[0] = '\0';

    //check if there is an open db file 
    res_open = openDatabase(curr_db);
    if(res_open == 0){
        printf("\nNo open db file.\n");
        return(1);
    }

    scanf(" %s %s", objname, fname);
    //check if the object name exists in the database
    res_search = findWrapper(curr_db, objname, 0);
    if(res_search == 0){
        printf("\nObject %s not in db.\n", objname);
        return 0;
    }
    //check if the file exists and creates it, if not
    file_pointer = createFile(fname);
    if(file_pointer == NULL){
        printf("Cannot open file %s.\n", fname);
        return(0);
    }
    exportFile(curr_db->file, file_pointer);
    fclose(file_pointer);
    
    return 0;
}

int deleteWrapper(curr_dbT* curr_db){
    int res_open, res_search;
    char objname[256];

    res_open = openDatabase(curr_db);
    if(res_open == 0){
        printf("\nNo open db file.\n");
        return 0;
    }

    scanf(" %s", objname);
    //check if the name exists in the db
    res_search = findWrapper(curr_db, objname, 0);
    if(res_search == 0){
        printf("\nObject %s not in db.\n", objname);
        return 0;
    }
    //at this point the file pointer is placed right after the object name 
    deleteFile(curr_db->file, objname, curr_db->dbname);
    curr_db->open_db = 0;
    openFile(curr_db->dbname, curr_db);
    return 0;
}

int closeWrapper(curr_dbT* curr_db){
    int res_open;
    
    res_open = openDatabase(curr_db);
    if(res_open == 0){
        printf("\nNo open db file.\n");
        return 0;
    }
    closeDatabase(curr_db);
    return 0;
}

void initFile (FILE* database_ptr){
    //the "dead_code" string is the meta-data used to verify if a file is a valid database file.
    char str[] = "dead_code";
    
    fwrite(str, sizeof(char), sizeof(str), database_ptr);
    fseek(database_ptr, 0L, SEEK_SET);
}

int openFile(char dbname[255], curr_dbT* curr) {
    char metadata_check[10];
    int res_read;
    
    metadata_check[0] = '\0';
    //Check if another database is already open and close it before opening a new one
    if(curr->open_db == 1){
        fclose( curr->file);
    }
    if( (curr->file = fopen( dbname, "r") ) ){
        //the file exists already
        //check if the db has the expected meta-data
        res_read = fread(metadata_check, 1, 9, curr->file);
        metadata_check[9] = '\0';
        if(res_read < 9){
            return(-1);
        }
        if(strcmp(metadata_check, "dead_code") == 0){
            fclose( curr->file);
            //reopen the base to read and write mode
            curr->file = fopen( dbname, "r+");
            fseek(curr->file, 0L, SEEK_SET);
            curr->open_db = 1;
            return(1);
        }
        return(-1);
    }
    //the fopen in read mode has failed, so the file is created and initialized
    curr->file = fopen( dbname, "w+");
    //check if the creation of an empty base failed
    if(curr->file == NULL){
        perror("Error: ");
        exit(EXIT_FAILURE);
    }
   
    initFile ( curr->file);
    curr->open_db = 1;
    return(1);
}
int openDatabase(curr_dbT* curr){

    if(curr->open_db == 1){
        return 1;
    }
    return 0;
}
void closeDatabase(curr_dbT* curr_db){

    fclose(curr_db->file);
    curr_db->open_db = 0;
    curr_db->dbname[0] = '\0';
}

long int countSize (FILE* fp){
    long int size;
    int res_seek;

    //find the end of file
    res_seek = fseek(fp, 0L, SEEK_END);
    if(res_seek != 0){
        perror("Error: ");
        exit(EXIT_FAILURE);
    }
    //using ftell func we calculate the total size of the object file
    size = ftell(fp);

    return(size);
}

int importFile (FILE* database_ptr, char filename[256], char objname[256]){
    char buff[512];
    long int size, written_bytes, remain, res_read;
    FILE* fp;
    int res_seek, len;

    //init the array. the array is used to import a file in smaller parts
    buff[0] = '\0';

    //open the object file in read mode, if it fails the file does not exist.
    fp = fopen(filename, "r");
    //check if the opening was successful
    if( fp == NULL ){
       return 0;
    }

    //copy the name of the object and the size before copying the content
    res_seek = fseek(database_ptr, 0L, SEEK_END);
    if(res_seek != 0){
        perror("Error: ");
        fclose(fp);
        exit(EXIT_FAILURE);
    }
    len = strlen(objname);
    //write the terminal '\0' byte to show the end of the object name
    objname[len] = '\0';
    //before importing the content of an object, write the name and the size
    fwrite (objname, sizeof(char), len+1, database_ptr);
    fflush(database_ptr);
    size = countSize(fp);
    fwrite(&size, sizeof(long int), 1, database_ptr);
    fflush(database_ptr);

    //import the file
    //object file with less than 512 bytes
    written_bytes = 0;
    remain = 0;
    //the obj file is open and the data are copied to the database using a 512b buffer
    while(size >= written_bytes){
        remain = size - written_bytes;
        buff[0] = '\0';
        
        //the data to be copied are less than 512 bytes
        if(remain <= 512){
            //move the file pointer right before the data to be transfered with the buffer
            fseek(fp, -remain, SEEK_END);
            res_read = fread(buff, sizeof(unsigned char), remain , fp);
            if(res_read != remain ){
                perror("Error: ");
                exit(EXIT_FAILURE);
            }
            fwrite( buff, sizeof(unsigned char), remain , database_ptr);
            fflush(database_ptr);
            break;
        }
        //the data to be copied are more than 512 bytes
        else{
            //move the file pointer right before the data to be transfered with the buffer
            fseek(fp, -remain, SEEK_END);
            //transfer 512 bytes at a time 
            res_read = fread(buff, sizeof(unsigned char), 512, fp);
            if( res_read != 512 ){
                perror("Error: ");
                exit(EXIT_FAILURE);
            }
            fwrite( buff, sizeof(unsigned char), 512, database_ptr);
            fflush(database_ptr);
            written_bytes += 512;
        }
    }
    fclose(fp);
    return(1);
}

int nameLen(FILE* database_ptr){
    int length, res_read, res_seek;
    char ch;

    length = 0;
    //first char of obj name
    res_read = fread(&ch, sizeof(char), 1, database_ptr);
    if(res_read == 0){
        return 0;
    }
    res_seek = fseek(database_ptr, 0L, SEEK_CUR);
    if( res_seek != 0 ){
        perror("Error: ");
        exit(EXIT_FAILURE);
    }
    if( ch != '\0'){
        length++;
    }
    else
        return(0);
    //count the length until reaching '\0' char
    while( ch != '\0' ){
        fread(&ch, sizeof(char), 1, database_ptr);
        //fseek(database_ptr, 0L, SEEK_CUR);
        length++;
    }
    //the database_ptr is placed before the string with the name of object file
    fseek(database_ptr, (long int) -length, SEEK_CUR);

    //the length returned contains also \0 character
    return(length);
}

char* objectDb(FILE* database_ptr){
    char *str = NULL;
    int bytes_read;

    //read the file name length
    bytes_read = nameLen(database_ptr);
    if(bytes_read == 0){
        return (str);
    }
    //allocate memory for the temporary string where we save object
    str = (char*) malloc(sizeof(char) * (bytes_read) );
    if(str == NULL){
        perror("Error: ");
        return 0;
    }
    str[0] = '\0';
    //read the file name
    fread(str, sizeof(char), bytes_read, database_ptr);    
    return (str);
}

int searchObjname(FILE* database_ptr, char objname[256]){
    char *str;
    long int file_size;

    //the first name is always found after "dead_code", so 10 bytes are ignored
    fseek(database_ptr, 10L, SEEK_SET);
    while(1){
        str = objectDb(database_ptr);
        if(str == NULL){
            free(str);
            return 0;
        }
        else if(str == 0){
            free(str);
            return 0;
        }
        //search for the string read in the string returned by objDb
        if(strcmp(str, objname) == 0){
            free(str);
            return(1);
        }
        free(str);
        //read the long int that indicates the number of bytes fo the file
        fread(&file_size, sizeof(long int), 1, database_ptr);
        //move the file pointer, to pass through the content of the file
        fseek(database_ptr, file_size, SEEK_CUR);
    }
    return 0;
}

void findStr (FILE* database_ptr, char str_read[256]){
    char *str;
    long int file_size;

    //the first name is always found after "dead_code", so 10 bytes are ignored
    fseek(database_ptr, 10L, SEEK_SET);
    printf("\n##\n");
    while(1){
        str =  objectDb(database_ptr);
        if(str == 0){
            free(str);
            break;
        }
        //search for the string read in the string returned by objDb
        if(strstr(str, str_read) != NULL){
            printf("%s\n", str);
        }
         //read the long int that indicates the number of bytes fo the file
        fread(&file_size, sizeof(long int), 1, database_ptr);
        //move the file pointer, to pass through the content of the file
        fseek(database_ptr, file_size, SEEK_CUR);
        free(str);
    }
}

void printAll(FILE* database_ptr){
    char *str;
    long int file_size;

    //the first name is always found after "dead_code", so 10 bytes are ignored
    fseek(database_ptr, 10L, SEEK_SET);
    printf("\n##\n");
    
    while(1){
        str = objectDb(database_ptr);
        if(str == 0){
            free(str);
            break;
        }
        printf("%s\n", str);
        //read the long int that indicates the number of bytes fo the file
        fread(&file_size, sizeof(long int), 1, database_ptr);
        //move the file pointer, to pass through the content of the file
        fseek(database_ptr, file_size, SEEK_CUR);
        free(str);
    }   
}

FILE* createFile(char fname[256]){
    FILE *file_pointer = NULL;

    //check if the file exists already
    file_pointer = fopen(fname, "r");
    //the file exists already 
    if(file_pointer != NULL){
        fclose(file_pointer);
        return(NULL);
    }
    //the file is opened in w+ mode, so as to be created and truncated to 0 length
    file_pointer = fopen(fname, "w+");
    if(file_pointer == NULL){
        perror("Error: ");
        return(NULL);
    }
    return(file_pointer);
}

int exportFile(FILE* database_ptr, FILE* file_pointer){
    long int file_size, written_bytes, remain, res_read, res_write;
    char buff[512];

    written_bytes = 0;
    remain = 0;
    buff[0] = '\0';
    //read the long int that indicates the number of bytes fo the file
    res_read = fread(&file_size, sizeof(long int), 1, database_ptr);
    if( res_read == 0 ){
        perror("Error: ");
        exit(EXIT_FAILURE);
    }

    //export the data from the database to the file in parts of 512 bytes
    while(written_bytes < file_size){
        remain = file_size - written_bytes;
        buff[0] = '\0';
		//the data to be extracted are less than 512 bytes
        if( remain <= 512){
            res_read = fread(buff, sizeof(char), remain, database_ptr);
            if(res_read != remain ){
                perror("Error: ");
                exit(EXIT_FAILURE);
            }
            res_write = fwrite(buff, sizeof(char), remain, file_pointer);
            if(res_write != remain){
                perror("Error: ");
                exit(EXIT_FAILURE);
            }
            fflush(file_pointer);
            break;
        }
		//the data to be extracted are more than 512 bytes
        else{ 
            res_read = fread(buff, sizeof(char), 512, database_ptr);
            if(res_read != 512 ){
                perror("Error: ");
                exit(EXIT_FAILURE);
            }
            res_write = fwrite(buff, sizeof(unsigned char), 512, file_pointer);
            if(res_write != 512){
                perror("Error: ");
                exit(EXIT_FAILURE);
            }
            fflush(file_pointer);
            written_bytes += 512;
        }
    }
    return(1);
}

int deleteFile(FILE* database_ptr, char objname[256], char dbname[256]){
    long int res_read, db_size, end_objfile, bytes_to_transf, file_size, name_len, bytes_to_del = 0;
    int res_seek;
    char buff[512];

    //calculate the total bytes to be deleted
    //adding 1 for the /0 char
    name_len = strlen( objname) + 1;
    //the pointer is placed after the end of objname
    fread(&file_size, sizeof(long int), 1, database_ptr);
    bytes_to_del = name_len + sizeof(long int) + file_size;
    //  printf("total bytes of the file to be deleted %ld\n", bytes_to_del);

    //calculate the bytes to be transfered in the inside of the db
    //move the file pointer so as to skip the objected to be deleted
    res_seek = fseek( database_ptr, file_size, SEEK_CUR);
    if(res_seek != 0){
        perror("Error: ");
        exit(EXIT_FAILURE);
    }
	//calculate the bytes from the begining of the database until the last bytes of the object to be deleted
    end_objfile = ftell(database_ptr);
    //move the pointer in the end of the db
    res_seek = fseek(database_ptr, 0L, SEEK_END);
    if(res_seek != 0){
        perror("Error: ");
        exit(EXIT_FAILURE);
    }
	//calculate the total bytes of the data base
    db_size = ftell(database_ptr);
	//calculate the actual size of bytes that need to be transfered
    bytes_to_transf = db_size - end_objfile;

    //place the pointer to the begining of the data that will be written over the deleted bytes 
    res_seek = fseek(database_ptr, -bytes_to_transf, SEEK_END);
    if(res_seek != 0){
        perror("Error: ");
        exit(EXIT_FAILURE);
    }

    while( bytes_to_transf != 0){
        buff[0] = '\0';

        if(bytes_to_transf <= 512){
            res_read = fread(buff, sizeof(char), bytes_to_transf, database_ptr);
            if(res_read != bytes_to_transf){
                perror("Error: ");
                exit(EXIT_FAILURE);
            }
			//move the file pointer before the name of the object to be deleted
            fseek(database_ptr, -(bytes_to_transf + bytes_to_del), SEEK_END);
            fwrite(buff, sizeof(char), bytes_to_transf, database_ptr);
            fflush(database_ptr);
            break;
        }
        else{
            res_read = fread(buff, sizeof(char), 512, database_ptr);
            if(res_read != 512){
                perror("Error: ");
                exit(EXIT_FAILURE);
            }
            //move the file ptr back 512 read bytes and also back to overwrite the previous content of the file deleted
            fseek(database_ptr, -(512 + bytes_to_del), SEEK_CUR);
            fwrite(buff, sizeof(char), 512, database_ptr);
            fflush(database_ptr);
            res_seek = fseek(database_ptr, bytes_to_del, SEEK_CUR);
            if(res_seek != 0){
                perror("Error: ");
                exit(EXIT_FAILURE);
            }
            bytes_to_transf = bytes_to_transf - 512;
        }
    }
    //close the database before calling truncate func
    fclose(database_ptr);
    //truncate the file so the size was the initial decreased by the total size of the objfile, the name and the size
    truncate( dbname, (db_size - bytes_to_del));
    
    return 0;
}