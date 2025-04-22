#include "exo6.h"

int main(){

    printf("Welcome to your CPU simulator !\n");
    printf("[Press Enter to continue...]\n\n");
    while (getchar() != '\n'); 
    usleep(400000);
    printf("Select an option (1-2): \n");
    printf("╔════════════════════════════╗\n");
    printf("║    CPU SIMULATOR MENU      ║\n");
    printf("╠════════════════════════════╣\n");
    printf("║ 1. Initialize CPU          ║\n");
    printf("║ 2. Exit                    ║\n");
    printf("╚════════════════════════════╝\n");
    int ipt=input(1,2);
    
    switch(ipt){
        case 1:
            break;
        case 2:
            printf("Exiting simulator. Goodbye!\n\n");
            usleep(1700000);
            exit(0); 
    }
    usleep(500000);
    printf("Select CPU memory size (128-1024): \n");
    ipt=input(128,1024);

    cpu_init(ipt);

    printf("Select an option (1-2): \n");
    printf("╔════════════════════════════╗\n");
    printf("║    CPU SIMULATOR MENU      ║\n");
    printf("╠════════════════════════════╣\n");
    printf("║ 1. Parse .txt file         ║\n");
    printf("║ 2. Show CPU                ║\n");
    printf("║ 3. Allocate Extra Segment  ║\n");
    printf("║ 4. Exit                    ║\n");
    printf("╚════════════════════════════╝\n");
    int ipt=input(1,4);

    switch(ipt){
        case 1:
            printf("Select CPU memory size (128-1024): \n");
            //scanf le .txt
            //appelle la fct
            //dmd si il veut voir le parser 

            break;

            
        case 4:
            printf("Exiting simulator. Goodbye!\n\n");
            usleep(1700000);
            exit(0); 
    }
    usleep(500000);
    printf("Select CPU memory size (128-1024): \n");
    ipt=input(128,1024);








    return 0;

}
    


    

















