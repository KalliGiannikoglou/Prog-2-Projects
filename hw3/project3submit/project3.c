#include "objdb.h"

int main(int argc, char *argv[]){
    char opt, str[256];
    curr_dbT curr;
    curr_dbT* curr_db;
   
    curr.open_db = 0;
    curr.file = NULL;
    curr.dbname[0] = '\0';
    curr_db = &curr;

    while(1){
        scanf(" %c", &opt);
        switch(opt){
            case 'o':{

                openWrapper(curr_db);
                break;
            }
            case 'i':{

                importWrapper( curr_db);
                break;
            }
            case 'f':{
                str[0] = '\0';
                findWrapper(curr_db, str, 1);
                break;
            }
            case 'e':{

                exportWrapper(curr_db);
                break;
            }
            case 'd':{
                deleteWrapper(curr_db);
                break;
            }
            case 'c':{
               closeWrapper(curr_db);
               break;
            }
            case 'q':{
                if(curr_db->open_db == 1){
                    closeWrapper(curr_db);
                }
                return 0;
            }
        }
    }
    return 0;
}
