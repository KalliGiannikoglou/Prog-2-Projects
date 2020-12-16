//Giannikoglou Kalliopi AEM:02915
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

//data type for student 
typedef struct student{
    char name[64];
    long unsigned int AEM;
    short unsigned int remain_courses;
}studentT;

typedef struct array{
    
    studentT** ptr_array;
    int size;
    int initial_size; 
    int deviation_size;
    int sorted;
    int valid_entries;
}arrayT;

//argument sorted defines whether the search will be linear or binary. 
//the function returns -1 if the aem was not found or the position of the aem inside the ptr array
int find(long unsigned int aem, arrayT* struct_pointer, int find_func); 

//searches for the aem, if found returns -1, else searches for empty space and if not found allocates extra memory and returns 1
int add(long unsigned int aem, short unsigned int courses, char stud_name[64], arrayT* struct_pointer, int find_func);

//prints the entries using the pointer array
void print(arrayT* struct_pointer);

//searches for the current aem. if found modifies the number of remaining courses and returns 1, else returns -1
int mod(arrayT* struct_pointer, long unsigned int aem, short unsigned int current_courses, int find_func);

//searches for the current aem. if not found returns -1 else free the memory of the current student struct, turns the according pointer to NULL 
// rearranges the pointers so as to have the null pointers in the end of the array and returns the number of comparisons
int rmv(arrayT* struct_pointer, long unsigned int aem, int find_func);

//deletes all the entries, frees the according memory as well as the pointer array
void clear(arrayT* struct_pointer);

//sorts the entries in increasing order using the pointer array
void sort(arrayT* struct_pointer);

int main(int argc, char* argv[]){
   
    int i;
    int initial_size,deviation_size;
    char opt;
    arrayT* struct_pointer;
    int find_func=0;
    long unsigned int aem;
    short unsigned int courses, current_courses;
    char stud_name[64];
    int res_f, res_m, res_a,res_r;

    initial_size = atoi(argv[1]);
    deviation_size = atoi(argv[2]);
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

    while(1){
        scanf("%c", &opt);
        
        switch (opt){

            case 'a':{
                find_func = 0;
                struct_pointer->sorted = 0;
                scanf(" %ld %s %hd", &aem, stud_name, &courses);
                res_a = add( aem, courses, stud_name, struct_pointer, find_func);
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
                res_r = rmv(struct_pointer, aem, find_func);
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

                scanf(" %ld", &aem);
                find_func = 1;
                res_f = find( aem, struct_pointer, find_func);
                if(res_f == -1){
                    printf("\nF-NOK %ld\n", aem);
                }
                else{
                    printf("\nF-OK %s %d\n", struct_pointer->ptr_array[res_f]->name, struct_pointer->ptr_array[res_f]->remain_courses);
                }
                break;
            }
            case 'p':{

                print(struct_pointer);
                break;
            }
            case 'c':{
                clear(struct_pointer);
                printf("\nC-OK\n");
                break;
            }
            case 'q':{
                clear(struct_pointer);
                free(struct_pointer);
                return 0;
            }
        }
    }

    return 0;
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

void clear(arrayT* struct_pointer){
    int i;

    for(i=0; i<struct_pointer->valid_entries; i++){
        free(struct_pointer->ptr_array[i]);
    }
    struct_pointer->size = 0;
    struct_pointer->valid_entries = 0;
    free(struct_pointer->ptr_array);
   
}

int rmv(arrayT* struct_pointer, long unsigned int aem, int find_func){
    int res_f;
    int count_null=0;
    studentT** temp = NULL;

    res_f = find(aem, struct_pointer, find_func);

    //the aem was not found
    if(res_f == -1){
        return(-1);
    }

    //the aem was found
    //remove the memory of the according struct
    free(struct_pointer->ptr_array[res_f]);
    //turn the pointer to NULL
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
        struct_pointer->size = struct_pointer->size - struct_pointer->deviation_size;

        if(temp == NULL){
        return 0;
        }
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

void print(arrayT* struct_pointer){
    int i;

    printf("\n#\n");
    for(i=0; i<struct_pointer->valid_entries; i++){
         printf("%ld %s %d\n", struct_pointer->ptr_array[i]->AEM, struct_pointer->ptr_array[i]->name, struct_pointer->ptr_array[i]->remain_courses);
    }
}


int add(long unsigned int aem, short unsigned int courses, char stud_name[64], arrayT* struct_pointer, int find_func){
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
   
    while(stud_name[j] != '\0'){
        stud_name[j] = toupper(stud_name[j]);
        j++;
    }
    strcpy(struct_pointer->ptr_array[struct_pointer->valid_entries]->name, stud_name);

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
