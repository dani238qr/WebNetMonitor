#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
    #define OS_WINDOWS 1
    #define WAIT_TIME 2000
#else
    #include <unistd.h>
    #define OS_WINDOWS 0
    #define WAIT_TIME 1
#endif

int main(int argc, char *argv[]) {

    if(argc !=2 ){
        fprintf(stderr, "add h for help\n");
        return 1;
    }

    if (OS_WINDOWS) {
        printf("Windows\n");
    }
    else{
        system("chmod +x src/linux_src.sh");
    }


        if (OS_WINDOWS) {
            printf("WINDOES\n");
    }
            
    else {
        if (strcmp(argv[1],"h")==0){
            system("./src/linux_src.sh h");
        }
        else if (strcmp(argv[1],"ip")==0){
            system("./src/linux_src.sh ip");
        }
        else if (strcmp(argv[1],"web")==0){
            system("./src/linux_src.sh web");
        }
        else if (strcmp(argv[1],"all")==0){
            system("./src/linux_src.sh all");
        }
        else{
            fprintf(stderr, "add h for help\n");
            return 1;
        }
        
    }

    return 0;

}
