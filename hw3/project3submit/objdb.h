#ifndef _OBJDB_H
#define _OBJDB_H
#include <stdio.h>

typedef struct curr_db{
    int open_db;
    FILE *file;
    char dbname[256];
} curr_dbT;

//manages the opening of the database files
void openWrapper(curr_dbT* curr_db); 
//manages the import of a file in the database
int importWrapper(curr_dbT* curr_db);
//manages the search for a file name in the database file. when called from another func the flag is 0 and 1 if called by the 
//user. returns 1 if an objname already exists in the db, else returns 0
int findWrapper(curr_dbT* curr_db, char objname[256], int find_flag);
//manages the export from the database to another file
int exportWrapper(curr_dbT* curr_db);
//manages the delete of a file from the database file
int deleteWrapper(curr_dbT* curr_db);
//manages the closure of the current database, if any open
int closeWrapper(curr_dbT* curr_db);

//open a db. checks if the given file exists already we check for apropriate file type, if it doesn't exist a new file is created 
//and initialized if the file exists but doesn't have the appropriate metadata returns -1, 
//if fopen fails returns 0, else 1 for success
int openFile(char dbname[255], curr_dbT* curr);
//init the new database with the apropriate metadata
void initFile (FILE* database_ptr);
//check if there is an open database file. If no db file is open return 0, else returns 1
int openDatabase(curr_dbT* curr);
//close the current database file, if there is one open
void closeDatabase(curr_dbT* database_ptr);
//calculate the size of an object file
long int countSize (FILE* fp);
//import a file in the database. if the file does not exist returns 0, if there is already a file with the same name in the database
//returns -1, else returns 1.
int importFile (FILE* database_ptr, char filename[256], char objname[256]);
//calculate the size of an object name
int nameLen(FILE* database_ptr);
//reads the name of an object in the database and returns the string read.
char* objectDb(FILE* database_ptr);
//prints all the names of the files in the database
void printAll(FILE* database_ptr);
//print the names of the files that include a specific read string 
void findStr (FILE* database_ptr, char str_read[256]);
//search the database for an object name, returns 1 if the name exists already, else 0 
int searchObjname(FILE* database_ptr, char objname[256]);
//called by exportWrapper. Checks if the given file name already exists. if yes, returns NULL. else a file with the given name 
//is created and returns a pointer to the file created.
FILE* createFile(char fname[256]);
//export the content of an object from the database to the new file. terminate if a stdio function fails, 1 for successful export
int exportFile(FILE* database_ptr, FILE* file_pointer);
//delete a file from the database. terminate if stdio functions fail
int deleteFile(FILE* database_ptr, char objname[256], char dbname[256]);

#endif
