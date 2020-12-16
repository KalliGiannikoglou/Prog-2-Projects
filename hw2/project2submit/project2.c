//Giannikoglou Kalliopi AEM:02915
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

//valgrind --leak-check=full --track-origins=yes -v --show-leak-kinds=all --leak-resolution=high 
//data type for the linked list of registrered courses 
typedef struct courses{

    short unsigned int code;
    struct courses *next;
}coursesT;

//data type for student 
typedef struct student{

    char name[64];
    long unsigned int AEM;
    short unsigned int remain_courses;
    coursesT* head;
    struct student* next;
    struct student* prev;
}studentT;

//data type for pointer array
typedef struct array{
    
    studentT** ptr_array;
    int size;
    int initial_size; 
    int deviation_size;
    int sorted;
    int valid_entries;
}arrayT;

//data type for hash table
typedef struct hash{

    studentT** hash_table;
    unsigned long size;
    unsigned long initial_size;
    float load_factor;
    unsigned long elements;
}hashT;


//argument sorted defines whether the search will be linear or binary. 
//the function returns -1 if the aem was not found, else returns the position of the aem inside the ptr array
int find(long unsigned int aem, arrayT* struct_pointer, int find_func); 
//searches for the aem, if found returns -1, else searches for empty space and if not found allocates extra memory and returns 1
int add(long unsigned int aem, short unsigned int courses, char stud_name[64], arrayT* struct_pointer,  hashT *hash_pointer,int find_func);
//prints the entries using the pointer array
void print(hashT *hash_pointer);
//searches for the current aem. if found modifies the number of remaining courses and returns 1, else returns -1
int mod(arrayT* struct_pointer, long unsigned int aem, short unsigned int current_courses, int find_func);
//searches for the current aem. if not found returns -1 else free the memory of the current student struct, turns the according pointer to NULL 
// rearranges the pointers so as to have the null pointers in the end of the array and returns the number of comparisons
int rmv(arrayT* struct_pointer, long unsigned int aem, int find_func, hashT *hash_pointer);
//deletes all the entries, frees the according memory as well as the pointer array
void clear(arrayT* struct_pointer, hashT *hash_pointer);
//sorts the entries in increasing order using the pointer array
void sort(arrayT* struct_pointer);
//searches the list of courses of one student to find a specific course code. if found returns 1, else -1
int search_list(coursesT* head, short unsigned int code);
//add an element in the list of courses. if the aem is not found returns -1,if the course is already in the registration list the func returns 0 else returns 1 
int reg_course(arrayT* struct_pointer, long unsigned int aem, short unsigned int code, int find_func); 
//prints the list of courses one student is registered. if the student does not exist the func returns -1, else 1
void list_courses(arrayT* struct_pointer, long unsigned int aem, int find_func);
//searches the array of students, returns -1 if the aem is not found. if the aem exists, checks if the student is registered in the course. returns 1 if found, else 0  
int is_reg(arrayT* struct_pointer, long unsigned int aem, short unsigned int code, int find_func);
//searches for a code is reg list of a student. if the student is not in the array returns -1, if the course does not exist returns 0, if found and removed returns 1
int unreg_course(arrayT* struct_pointer, long unsigned int aem, short unsigned int code, int find_func);
//initialize the hash table. allocate memory for the sentinel and arrange the pointers
void init(hashT* hash_pointer, long unsigned size);
//the argument is a name and returns an int that is turned into a hash value
unsigned long hash(char *str);
//divides the value of hash func with the size of hash table and rearrenge pointers so as students are linked in alphabetical order. if two students have the same name 
//the link is in non decreasing order
void add_hash(hashT *hash_pointer, studentT *new_stud);
//sorts the double link list in alphabetical order
int bubble_sort(studentT *head);
//search the bucket list to find a name. if found returns 1, else 0
int find_name(hashT *hash_pointer, char name[64]);
//prints all the aem and remaining courses of students with the same name
void print_name(hashT *hash_pointer, char name[64]);
//remove a node from the hash table
void remove_list(hashT *hash_pointer, char name[64], long unsigned aem);
//delete the connecting pointers of the nodes so as the memory of the hash table
void clear_list(hashT *hash_pointer);
//clear all the allocated memory and terminate the program
void quit(arrayT *struct_pointer, hashT *hash_pointer);
//change the size of the hash table according to the load factor
void rehash(hashT *hash_pointer);

int main(int argc, char* argv[]){
   
    int i;
    int initial_size,deviation_size;
    unsigned long hash_initial;
    char opt;
    arrayT* struct_pointer;
    int find_func=0;
    long unsigned int aem;
    short unsigned int courses, current_courses;
    char stud_name[64];
    int res_f, res_m, res_a,res_r,res_g,res_i, res_u, res_name;
    short unsigned int code;
    hashT *hash_pointer;
    struct timespec start, end;
    double res_time;

    initial_size = atoi(argv[1]);
    deviation_size = atoi(argv[2]);
    hash_initial = atol(argv[3]);
    //allocate memory for the arrayT struct
    struct_pointer = (arrayT*) malloc(1*sizeof(arrayT) );
    //allocate memory for the pointer array according to the first given argument
    struct_pointer->ptr_array = (studentT**) malloc(sizeof(studentT*) * initial_size);

    //supposing the given arguments are valid define the initial size of pointer array 
    //as long as the deviation size of the pointer array
    struct_pointer->initial_size = initial_size;
    struct_pointer->deviation_size = deviation_size;
    struct_pointer->valid_entries = 0;
    //initialize the dynamically allocted array
    struct_pointer->size = initial_size;
    for(i=0; i< struct_pointer->size; i++){
        struct_pointer->ptr_array[i] = NULL;
    }
    //allocate memory for the struct hashT
    hash_pointer = (hashT*)malloc(1*sizeof(hashT));
    //allocate memory for the hash table
    hash_pointer->hash_table = (studentT**) malloc(sizeof(studentT*)*hash_initial);
    hash_pointer->initial_size = hash_initial;
    
    //initialize the hash struct and the double linked list
    init(hash_pointer, hash_initial);

    while(1){
        scanf("%c", &opt);
        
        switch (opt){

            case 'a':{
                find_func = 0;
                struct_pointer->sorted = 0;
                scanf(" %ld %s %hd", &aem, stud_name, &courses);
                res_a = add( aem, courses, stud_name, struct_pointer, hash_pointer,find_func);
                if( res_a == 0){
                    return 0;
                }
                else if(res_a == -1){
                    printf("\nA-NOK %ld, %d %d\n", aem, struct_pointer->valid_entries, struct_pointer->size);
                }
                else{
                    printf("\nA-OK %ld, %d %d\n", aem, struct_pointer->valid_entries, struct_pointer->size);
                }

                break;
            }
            case 'r':{
                
                find_func = 0;
                struct_pointer->sorted = 0;
                scanf(" %ld", &aem);
                res_r = rmv(struct_pointer, aem, find_func, hash_pointer);
                if(res_r == 0){
                    printf("\nDONE\n");
                    return 0;
                }
                if(res_r == -1){
                    printf("\nR-NOK %ld, %d %d\n", aem, struct_pointer->valid_entries, struct_pointer->size);
                }
                else{
                    printf("\nR-OK %ld, %d %d\n", aem, struct_pointer->valid_entries, struct_pointer->size);
                }
                break;
            }
            case 'm':{

                find_func=0;
                scanf(" %ld %hd", &aem, &current_courses);
                res_m = mod(struct_pointer, aem, current_courses, find_func);
                if( res_m == -1 ){
                    printf("\nM-NOK %ld\n", aem);
                }
                else{
                    printf("\nM-OK %ld\n", aem);
                }

                break;
            }
            case 's':{
                sort(struct_pointer);
               
                break;
            }
            case'f':{
                
                res_time = 0;
                scanf(" %ld", &aem);
                find_func = 1;
                clock_gettime(CLOCK_MONOTONIC, &start);
                res_f = find( aem, struct_pointer, find_func);
                clock_gettime(CLOCK_MONOTONIC, &end);
                res_time = (end.tv_sec - start.tv_sec) * 1e9; 
                res_time = (res_time + (end.tv_nsec - start.tv_nsec)) * 1e-9; 
                fprintf(stderr, "binary search time: %lf\n", res_time);
                if(res_f == -1){
                    printf("\nF-NOK %ld\n", aem);
                }
                else{
                    printf("\nF-OK %s %d\n", struct_pointer->ptr_array[res_f]->name, struct_pointer->ptr_array[res_f]->remain_courses);
                }
                break;
            }
            case 'p':{

                print(hash_pointer);
                break;
            }
            case 'c':{
                clear(struct_pointer, hash_pointer);
                printf("\nC-OK\n");
                break;
            }
            case 'g':{

                scanf(" %ld %hd", &aem, &code);
                find_func = 0;
                res_g = reg_course(struct_pointer, aem, code, find_func);
                if(res_g == -1){
                    printf("\nG-NOK %ld\n", aem);
                }
                else if( res_g == 0){
                    printf("\nG-NOK %hd\n", code);
                }
                else if(res_g == -2){
                    return 0;
                }
                else{
                    printf("\nG-OK %ld %hd\n", aem, code);
                }
                break;
            }
            case 'l':{
                
                scanf(" %ld", &aem);
                find_func = 0;
                list_courses( struct_pointer, aem, find_func);
                break;
            }
            case 'i':{
                
                scanf(" %ld %hd", &aem, &code);
                find_func = 0;
                res_i = is_reg(struct_pointer, aem, code, find_func);
                if(res_i == -1){
                    printf("\nI-NOK %ld\n", aem);
                }
                else if(res_i == 0){
                    printf("\nNO\n");
                }
                else{
                    printf("\nYES\n");
                }

                break;
            }
            case 'u':{
                 
                scanf(" %ld %hd", &aem, &code);
                find_func = 0;
                res_u = unreg_course(struct_pointer, aem, code, find_func);
                if(res_u == -1){
                    printf("\nU-NOK %ld\n", aem);
                }
                else if(res_u == 0){
                    printf("\nU-NOK %hd\n", code);
                }
                else{
                    printf("\nU-OK %ld %hd\n", aem, code);
                }
                break;
            }
            case 'n':{
                
                res_time = 0;
                scanf(" %s", stud_name);
                i = 0;
               while(stud_name[i] != '\0'){
                    stud_name[i] = toupper(stud_name[i]);
                    i++;
                }
                clock_gettime(CLOCK_MONOTONIC, &start);
                res_name = find_name(hash_pointer, stud_name);
                clock_gettime(CLOCK_MONOTONIC, &end);
                res_time = (end.tv_sec - start.tv_sec) * 1e9; 
                res_time = (res_time + (end.tv_nsec - start.tv_nsec)) * 1e-9; 
                fprintf(stderr, "hash table search time: %lf\n", res_time);
                if(res_name == 0){
                    printf("\nN-NOK %s\n", stud_name);
                    break;
                }
                printf("\nN-OK %s\n", stud_name);
                print_name(hash_pointer, stud_name);
                break;
            }
            case 'q':{
                quit(struct_pointer, hash_pointer);
                return 0;
            }
        }
    }

    return 0;
}
void quit(arrayT *struct_pointer, hashT *hash_pointer){
    int i;

    clear(struct_pointer, hash_pointer);
    free(struct_pointer);
    //free the memory of sentinel buckets
    for(i=0; i<hash_pointer->size; i++){
        free(hash_pointer->hash_table[i]);
    }
    free(hash_pointer->hash_table);
    free(hash_pointer);
}

void sort(arrayT* struct_pointer){
    studentT* temp;
    int i, j;
    long long int comparisons=0;

    //insertion sort: begin from the pos 1 of the pointer array and compare with the previous element, using a temp to swap the elements
    for(i=1; i<struct_pointer->valid_entries; i++){
       
        j = i;
        comparisons++;

        while( (j > 0) && (struct_pointer->ptr_array[j-1]->AEM > struct_pointer->ptr_array[j]->AEM ) ){
           
            temp = struct_pointer->ptr_array[j]; 
            struct_pointer->ptr_array[j] = struct_pointer->ptr_array[j-1];
            struct_pointer->ptr_array[j-1] = temp;
           //comparisons increase for each repetion except for the first since the iterator has already been increased(178 line) 
            if( j != i ){
                comparisons++;
            }
             j = j - 1;
        }
    }
    printf("\nS-OK\n");
    fprintf(stderr, "\n$%lld\n", comparisons); 

    struct_pointer->sorted = 1;
}

void clear(arrayT* struct_pointer, hashT *hash_pointer){
    int i;
    coursesT *curr,*temp;

    //the array is not empty
    if(struct_pointer != NULL){
        for(i=0; i<struct_pointer->valid_entries; i++){
            //remove all the nodes of the list of courses of each student
            for(curr = struct_pointer->ptr_array[i]->head; curr != NULL; curr = curr->next){
                temp = curr;
                free(temp);
            }
            clear_list(hash_pointer);
            //free the memory of each student
            free(struct_pointer->ptr_array[i]);
        }
        struct_pointer->size = 0;
        struct_pointer->valid_entries = 0;
        //free the memory of the pointer array
        free(struct_pointer->ptr_array);
    }
   
}

int rmv(arrayT* struct_pointer, long unsigned int aem, int find_func, hashT *hash_pointer){
    int res_f;
    int count_null=0;
    studentT** temp = NULL;
    coursesT *curr, *rmv_node;

    res_f = find(aem, struct_pointer, find_func);

    //the aem was not found
    if(res_f == -1){
        return(-1);
    }

    //the aem was found
    //remove all the nodes of the list of courses of the student
    curr = struct_pointer->ptr_array[res_f]->head;
    while( curr != NULL){
        rmv_node = curr;
        curr = curr->next;
       
        free(rmv_node);
    }
    //remove the node from the hash table
    remove_list(hash_pointer, struct_pointer->ptr_array[res_f]->name, struct_pointer->ptr_array[res_f]->AEM);

    //remove the memory of the according struct
    free(struct_pointer->ptr_array[res_f]);
    //turn the struct pointer to NULL
    struct_pointer->ptr_array[res_f] = NULL;

    //count the number of valid entries after removing one
    struct_pointer->valid_entries--;
    
    //count the null pointers in pointer array
    count_null = struct_pointer->size - struct_pointer->valid_entries;

    //res_f is the return of find so the position where we removed the entry
    struct_pointer->ptr_array[res_f] = struct_pointer->ptr_array[struct_pointer->valid_entries];
    struct_pointer->ptr_array[struct_pointer->valid_entries] = NULL;

    //reallocate the pointer array if the NULL pointers are more than the deviation size
    if(struct_pointer->deviation_size < count_null){
        temp = (studentT**) realloc(struct_pointer->ptr_array, (struct_pointer->size - struct_pointer->deviation_size) * sizeof(studentT*));
        if(temp == NULL){
         return 0;
        }
        struct_pointer->ptr_array = temp;
        struct_pointer->size = struct_pointer->size - struct_pointer->deviation_size;
    }
    
    //calculate the load factor of the hash table and if it equals 1 shorten the size
    hash_pointer->load_factor =(float) hash_pointer->elements / (float)hash_pointer->size;
    if(hash_pointer->load_factor == 1){
        rehash(hash_pointer);
    }
    return(1);
}

int mod(arrayT* struct_pointer, long unsigned int aem, short unsigned int current_courses, int find_func){
    int res_f;

    res_f = find(aem, struct_pointer, find_func);
    if( res_f == -1 ){
        return (-1);
    }

    //modify the remaining courses in the position where the aem was found
    struct_pointer->ptr_array[res_f]->remain_courses = current_courses;
    
    return 1;
}
int largest_bucket(hashT* hash_pointer){
    int l_bucket,temp,i;
    studentT *curr;

    l_bucket = 0;
    temp = 0;
    for(i=0; i<hash_pointer->size; i++){
        temp = 0;
        for(curr = hash_pointer->hash_table[i]->next; curr != hash_pointer->hash_table[i]; curr = curr->next){
            temp++;
        }
        if(temp > l_bucket){
            l_bucket = temp;
        }
    }
    return(l_bucket);
}
void print(hashT *hash_pointer){
    int i,bucket_length=0;
    studentT *curr;

    hash_pointer->load_factor = (float)hash_pointer->elements /(float) hash_pointer->size;
    printf("\n#\n%ld %ld %0.2f %d\n", hash_pointer->size, hash_pointer->elements, hash_pointer->load_factor, largest_bucket(hash_pointer));
    for(i=0; i<hash_pointer->size; i++){
        bucket_length=0;
        for(curr = hash_pointer->hash_table[i]->next; curr != hash_pointer->hash_table[i]; curr = curr->next){
            bucket_length++;
        }
        printf("%d %d", i, bucket_length);
        if(hash_pointer->hash_table[i]->next->AEM != 0){
            curr = hash_pointer->hash_table[i]->next;
            while(curr != hash_pointer->hash_table[i]){
                printf(" [%ld %s %hd]", curr->AEM, curr->name, curr->remain_courses);
                curr = curr->next;

            }
        }
        printf("\n\n");
    }
}
int add(long unsigned int aem, short unsigned int courses, char stud_name[64], arrayT* struct_pointer, hashT *hash_pointer,int find_func){
    int res;
    int j=0;
    studentT *entry;
    studentT **new;

    res = find(aem, struct_pointer, find_func);

    //the according aem already exists  
    if(res != -1){
        return (-1);
    }
    //edge case after calling clear function and struct_pointer->ptr_array = NULL
    if(struct_pointer->size == 0){
        //allocate memory for the pointer array according to the first given argument
        struct_pointer->ptr_array = (studentT**) malloc(sizeof(studentT*) * struct_pointer->initial_size);
    }
    //there is no NULL pointer in pointer array. allocation of extra memory
    if(struct_pointer->valid_entries == struct_pointer->size){

         //change the size of pointer array
        struct_pointer->size += struct_pointer->deviation_size;
        //reallocate memory for the pointer array according to the second given argument
        new = (studentT**) realloc(struct_pointer->ptr_array, (struct_pointer->size) * sizeof(studentT*));
       
        //the reallocation failed
        if(new == NULL){
            return 0;
        }
        struct_pointer->ptr_array = new; 
    }
    //allocate memory for the student struct
    entry = (studentT*) malloc(1*sizeof(studentT));

    //check whether the memory allocation was successful
    if(entry == NULL) {
        return 0;
    }   
    //valid entries is the first no NULL position 
    struct_pointer->ptr_array[struct_pointer->valid_entries] = entry;
    struct_pointer->ptr_array[struct_pointer->valid_entries]->AEM = aem;
    struct_pointer->ptr_array[struct_pointer->valid_entries]->remain_courses = courses;
    //init the head of the list to NULL
    struct_pointer->ptr_array[struct_pointer->valid_entries]->head = NULL;
    struct_pointer->ptr_array[struct_pointer->valid_entries]->next = NULL;
    struct_pointer->ptr_array[struct_pointer->valid_entries]->prev = NULL;
   
    while(stud_name[j] != '\0'){
        stud_name[j] = toupper(stud_name[j]);
        j++;
    }
    strcpy(struct_pointer->ptr_array[struct_pointer->valid_entries]->name, stud_name);

    add_hash(hash_pointer, struct_pointer->ptr_array[struct_pointer->valid_entries]); 
    struct_pointer->valid_entries++;
    //since a new entry was successful the array is no longer sorted
    struct_pointer->sorted = 0;  
    return(1);
}
    
int find(long unsigned int aem, arrayT* struct_pointer, int find_func){
    int i;
    int high,low,median;
    long long int comparisons=0;

    if(struct_pointer->valid_entries == 0){
        return(-1);
    }
    //if the array is not sorted the search will be linear
    if(struct_pointer->sorted == 0){
        for(i=0; i< struct_pointer->valid_entries; i++){

            comparisons++;
            if(struct_pointer->ptr_array[i]->AEM == aem){
                if(find_func == 1){
                    fprintf(stderr, "\n$%lld\n", comparisons);
                }
                return(i);
            }
        }
        if(find_func == 1){
            fprintf(stderr, "\n$%lld\n", comparisons);
        }
        return(-1);
    }
    //the array is sorted so the search is binary
    else{
        high = struct_pointer->valid_entries - 1;
        low = 0;

        while(low <= high){
            comparisons++;
            median = (high + low) / 2;
            if(aem == struct_pointer->ptr_array[median]->AEM){
                if(find_func == 1){
                    fprintf(stderr, "\n$%lld\n", comparisons);
                }
                return(median);
            }        
            if(aem > struct_pointer->ptr_array[median]->AEM){
                low = median + 1;
                comparisons++;
            }
            else {
                high = median - 1;
                comparisons++;
            }
        }
        if(find_func == 1){
            fprintf(stderr, "\n$%lld\n", comparisons);
        }
        return(-1);
    }
    return 0;
}
int search_list(coursesT* head, short unsigned int code){
    coursesT *curr;

    for(curr = head; (curr != NULL) && (curr->code != code); curr = curr->next);
    
    //if curr is NULL the course does not exist in the list courses
    if(curr == NULL){
        return(-1);
    }
    return(1);
}

int reg_course(arrayT* struct_pointer, long unsigned int aem, short unsigned int code, int find_func){
    int res_f,res_s,pos;
    coursesT *curr,*temp,*prev;    

    res_f = find(aem, struct_pointer, find_func);

    //the aem does not exist in the array 
    if(res_f == -1){
        return(-1);
    }

    //the student is already registered in the course
    pos = res_f;
    res_s = search_list(struct_pointer->ptr_array[pos]->head, code);
    if(res_s == 1){
        return 0;
    }

    //the course is added in the list
    //allocate memory for the course
    curr = (coursesT*) malloc( sizeof(coursesT) * 1);
    //check if the allocation succeeded
    if(curr == NULL){
        return(-2);
    }  
   //init the values
    curr->code = code;
    curr->next = NULL;
    temp = struct_pointer->ptr_array[pos]->head;
    prev = NULL;

    //entry in an empty list
    if(temp == NULL){
        curr->next = NULL;
        struct_pointer->ptr_array[pos]->head = curr;
        return(1);
    }

    //entry when the code is smaller than the code of the first element of the list
    else if(code < temp->code){

        curr->next = struct_pointer->ptr_array[pos]->head;
        struct_pointer->ptr_array[pos]->head = curr;
    }
    //the code is bigger than the first element of the list
    else{

        while( temp != NULL){
            if(code > temp->code){
                prev = temp;
                temp = temp->next;
                continue;
            }
            else{
                prev->next = curr;
                curr->next = temp;
                return(1);
            }
        }
        prev->next = curr;
    }
    return(1);
}
void list_courses(arrayT* struct_pointer, long unsigned int aem, int find_func){
    int res_f;
    coursesT* curr;

    //search if the student exists in the array 
    res_f = find( aem, struct_pointer, find_func);
    if(res_f == -1){
       
      printf("\nL-NOK %ld\n", aem);            
    }
    else{
        printf("\nL-OK %s\n", struct_pointer->ptr_array[res_f]->name);
        for(curr = struct_pointer->ptr_array[res_f]->head; curr != NULL; curr = curr->next){
            printf("%d\n", curr->code);
        }
    }
}
int is_reg(arrayT* struct_pointer, long unsigned int aem, short unsigned int code, int find_func){
    int res_f, res_g;

    res_f = find( aem, struct_pointer, find_func);
    if(res_f == -1){
        return(-1);
    }
    res_g = search_list(struct_pointer->ptr_array[res_f]->head, code);
    if(res_g == -1){
        return 0;
    } 
return(1);
}
int unreg_course(arrayT* struct_pointer, long unsigned int aem, short unsigned int code, int find_func){
    coursesT *curr,*prev;
    int res_i, res_f;

    res_i = is_reg(struct_pointer, aem, code, find_func);
    //the student does not exist in the pointer array
    if(res_i == -1){
        return(-1);
    }
    //the course does not exist in the list of the student
    else if(res_i == 0){
        return 0;
    }
    res_f = find( aem, struct_pointer, find_func);
    //the course exists and needs to be removed
    //we run through the list to find the node with the code asked
    for(curr = struct_pointer->ptr_array[res_f]->head, prev = NULL; (curr != NULL) && (curr->code != code); prev = curr, curr = curr->next);
        
    //the code to remove is the first of the list
    if(curr->code == struct_pointer->ptr_array[res_f]->head->code){
        struct_pointer->ptr_array[res_f]->head = curr->next;
        free(curr);
        return(1);
    } 
    prev->next = curr->next;
    free(curr);
    return(1);
}
void init(hashT* hash_pointer,long unsigned size){
    int i,j;

    //initialize hashT struct values
    hash_pointer->elements = 0;
    hash_pointer->load_factor = 0;
    hash_pointer->size = size;
   
    for(i=0; i<size; i++){
        //allocate memory for the sentinel    
        hash_pointer->hash_table[i] = (studentT*) malloc(sizeof(studentT));
        //next and prev point to head
        hash_pointer->hash_table[i]->next = hash_pointer->hash_table[i];
        hash_pointer->hash_table[i]->prev = hash_pointer->hash_table[i];
        for(j=0; j<64; j++){
            hash_pointer->hash_table[i]->name[j] = '\0';
        }
        hash_pointer->hash_table[i]->AEM = 0;
        hash_pointer->hash_table[i]->remain_courses = 0;
    }
}
unsigned long hash(char *str) {
    unsigned long hash = 5381; 
    int c; 

    while( (c = *str++) ){ 
      hash = ((hash << 5) + hash) + c; 
    }
    return hash; 
} 
void rehash(hashT *hash_pointer){
    studentT **new_table;
    int i,j;
    studentT *curr,*temp;
    unsigned long new_size=0,hash_value=0;

    //the array size 
    if(hash_pointer->load_factor >= 4){
        new_size = 2 * hash_pointer->size;
    }
    else if(hash_pointer->load_factor == 1){
        new_size = hash_pointer->size / 2;
        if(new_size < hash_pointer->initial_size){
            new_size = hash_pointer->initial_size;
        }
    }
    //allocate memory for the new hash table
    new_table = (studentT**)malloc(new_size * sizeof(studentT*));
    //initialize new table 
    for(i=0; i<new_size; i++){
        //allocate memory for the sentinel    
        new_table[i] = (studentT*) malloc(sizeof(studentT));
        //next and prev point to head
        new_table[i]->next = new_table[i];
        new_table[i]->prev = new_table[i];
        for(j=0; j<64; j++){
            new_table[i]->name[j] = '\0';
        }
        new_table[i]->AEM = 0;
        new_table[i]->remain_courses = 0;
    }

    for(i=0; i<hash_pointer->size; i++){
        curr = hash_pointer->hash_table[i]->next; 
        while(curr != hash_pointer->hash_table[i]){
            temp = curr->next;
            //calculate the new hash value of each student
            hash_value = hash(curr->name) % new_size;
            curr->next = new_table[hash_value]->next;
            curr->prev = new_table[hash_value];
            curr->next->prev = curr;
            curr->prev->next = curr;

            //sort the table
            bubble_sort(new_table[hash_value]);
            curr = temp;
        }
    }
    free(hash_pointer->hash_table);
    hash_pointer->hash_table = new_table;
    hash_pointer->size = new_size;
    hash_pointer->load_factor =(float) hash_pointer->elements / (float) hash_pointer->size;
}
void add_hash(hashT *hash_pointer, studentT *new_stud){
    unsigned long hash_value;

    //calculate the hash value
    hash_value = hash(new_stud->name) % hash_pointer->size;
    //add the new entry in the begining of the list 
    new_stud->next = hash_pointer->hash_table[hash_value]->next;
    new_stud->prev = hash_pointer->hash_table[hash_value];
    new_stud->next->prev = new_stud;
    new_stud->prev->next = new_stud;
    hash_pointer->elements++;
    
    //sort the list in alphabetical order
    bubble_sort(hash_pointer->hash_table[hash_value]);
    hash_pointer->load_factor =(float) hash_pointer->elements / (float) hash_pointer->size;
    if(hash_pointer->load_factor >= 4){
        rehash(hash_pointer);
    }
}
int bubble_sort(studentT *head){
    studentT *temp,*curr = head->next;

    //the list has only one student
    if(head->next->next == head){
        return 0;
    }
    //initialize the values
    curr = head->next;
    temp = curr->next;
    //check the alphabetical order of the names
    while(curr->next != head){
        //printf("\ncurr %ld next %ld", curr->AEM, curr->next->AEM);
        if(strcmp(curr->name, temp->name) < 0){
            return 0;
        }
        else if(strcmp(curr->name, temp->name) > 0){
            //the students need to swap
           curr->prev->next = temp;
           temp->next->prev = curr;
           curr->next = temp->next;
           temp->prev = curr->prev;
           curr->prev = temp;
           temp->next = curr; 
        }
        else{
        //two students have the same name
            if(curr->AEM > curr->next->AEM){
                //the students need to swap
                curr->prev->next = temp;
                temp->next->prev = curr;
                curr->next = temp->next;
                temp->prev = curr->prev;
                curr->prev = temp;
                temp->next = curr;            
            }          
        }
       curr = temp->next;
       temp = curr->next;
    }
    return 0;
}
void print_name(hashT *hash_pointer, char name[64]){
    unsigned long hash_value;
    studentT *curr;

    //calculate the hash value to search the according bucket of the hash table
    hash_value = hash(name) % hash_pointer->size;
    for(curr = hash_pointer->hash_table[hash_value]->next; curr != hash_pointer->hash_table[hash_value]; curr = curr->next){
        if(strcmp(curr->name, name) == 0){
            printf("%ld %hd\n", curr->AEM, curr->remain_courses);
        }
    }
}
int find_name(hashT *hash_pointer, char name[64]){
    unsigned long hash_value;
    studentT *curr;
    long long int comparisons;
    struct timespec start, end;
    double res_time;
    
    comparisons = 0;
    //calculate the hash value to search the according bucket of the hash table
    hash_value = hash(name) % hash_pointer->size;
     clock_gettime(CLOCK_MONOTONIC, &start);
    for(curr = hash_pointer->hash_table[hash_value]->next; curr != hash_pointer->hash_table[hash_value]; curr = curr->next){
         comparisons++;
        if(strcmp(name, curr->name) == 0){
            clock_gettime(CLOCK_MONOTONIC, &end);
            res_time = (end.tv_sec - start.tv_sec) * 1e9; 
            res_time = (res_time + (end.tv_nsec - start.tv_nsec)) * 1e-9; 
            fprintf(stderr, "hash table search time without hash value search: %.15lf\n", res_time);
            fprintf(stderr, "\n$$%lld\n", comparisons);
            
            return(1);
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    res_time = (end.tv_sec - start.tv_sec) * 1e9; 
    res_time = (res_time + (end.tv_nsec - start.tv_nsec)) * 1e-9; 
    fprintf(stderr, "hash table search time without hash value search: %lf\n", res_time);
    fprintf(stderr, "\n$$%lld\n", comparisons);
   
    return(0);
}
void remove_list(hashT *hash_pointer, char name[64], long unsigned aem){
    studentT *curr;
    long unsigned hash_value;

    //calculate the hash value to search the according bucket of the hash table
    hash_value = hash(name) % hash_pointer->size;
    for(curr = hash_pointer->hash_table[hash_value]->next; curr->AEM != aem; curr = curr->next);
    if(curr != hash_pointer->hash_table[hash_value]){
        curr->prev->next = curr->next;
        curr->next->prev = curr->prev;
    }
    hash_pointer->elements--;
}
void clear_list(hashT *hash_pointer){
   int i;
   
   //turn the double list to each initial size and rearrange the pointers of the sentinel
   for(i=0; i<hash_pointer->size; i++){
        hash_pointer->hash_table[i]->next =  hash_pointer->hash_table[i];
        hash_pointer->hash_table[i]->prev =  hash_pointer->hash_table[i];
   }
    hash_pointer->elements = 0;
    hash_pointer->load_factor = 0;
    hash_pointer->size = hash_pointer->initial_size;
}
