#include "exo6.h"

//clean terminal
//usleep time before menu or press ENTER?

int input(int min, int max) {
    int value;
    int success;
    char ch;

    do {
        success = scanf("%d", &value);

        if (success != 1) {
            // Invalid input: clear the buffer
            while ((ch = getchar()) != '\n' && ch != EOF);
            printf("Error: not a valid number. Try again.\n");
            continue;
        }

        if (value < min || value > max) {
            printf("Error: input value out of bounds. Try again. \n");
        }

    } while (success != 1 || value < min || value > max);

    return value;
}
 
CPU* menu1(){
    printf("\n-------[Press ENTER to continue...]-------\n\n");
    while (getchar() != '\n');
    printf("\033[2J\033[H"); 
    printf("\nSelect an option (1-2): \n");
    printf("╔════════════════════════════╗\n");
    printf("║    CPU SIMULATOR MENU      ║\n");
    printf("╠════════════════════════════╣\n");
    printf("║ 1. Initialize CPU          ║\n");
    printf("║ 2. Exit                    ║\n");
    printf("╚════════════════════════════╝\n");
    printf("\n");
    int ipt=input(1,2);
    
    switch(ipt){
        case 1:
            break;
        case 2:
            printf("Exiting simulator. Goodbye!\n\n");
            usleep(1300000);
            exit(0); 
    }
    usleep(500000);
    printf("\n Select CPU memory size (128-1024): \n");
    ipt=input(128,1024);

    CPU* cpu= cpu_init(ipt);
    if (!cpu) menu1();
    printf("\n CPU of size %d successfully initialized \n",ipt);
    usleep(1500000);
    return cpu;
}

ParserResult* menu2(CPU* cpu){
    int c;
    while ((c = getchar()) != '\n' && c != EOF); 
    printf("\n-------[Press ENTER to continue...]-------\n\n");
    while (getchar() != '\n');
    printf("\033[2J\033[H"); 
    printf("\nSelect an option (1-3): \n");
    printf("╔════════════════════════════╗\n");
    printf("║    CPU SIMULATOR MENU      ║\n");
    printf("╠════════════════════════════╣\n");
    printf("║ 1. Parse .txt file         ║\n");
    printf("║ 2. Show CPU                ║\n");
    printf("║ 3. Exit                    ║\n");
    printf("╚════════════════════════════╝\n");
    printf("\n");
    int ipt=input(1,3);

    switch(ipt){
        case 1:
            printf("\nENTER your file name with .txt extension \n  (pseudo-assembly program with .DATA and .CODE sections) \n");
            char filename[100];
            scanf("%s", filename);
            int len = strlen(filename);
            if (!(len >= 4 || strcmp(filename + len - 4, ".txt") == 0)){
                printf("File name in wrong format. ENTER a file name with .txt extension \n");
                usleep(1300000);
                menu2(cpu);
            }
            ParserResult * parser=parse(filename);
            if (!parser) {
                printf("Error parsing.\n");
                usleep(2600000);
                menu2(cpu);
            }
            
            return parser;
            break;
        
        case 2:
            print_cpu(cpu);
            printf("\n");
            break;

        case 3:
            printf("Exiting simulator. Goodbye!\n\n");
            cpu_destroy(cpu);
            usleep(1300000);
            exit(0); 
    }
    return NULL;
}

void menu3(CPU* cpu, ParserResult* parser){
    int c;
    while ((c = getchar()) != '\n' && c != EOF); 
    printf("\n-------[Press ENTER to continue...]-------\n\n");
    while (getchar() != '\n');
    printf("\033[2J\033[H"); 
    printf("\nSelect an option (1-6): \n");
    printf("╔═══════════════════════════════════════════════════════════╗\n");
    printf("║                    CPU SIMULATOR MENU                     ║\n");
    printf("╠═══════════════════════════════════════════════════════════╣\n");
    printf("║ 1. Allocate Variables (Data Segment)                      ║\n");
    printf("║ 2. Allocate Code Segment                                  ║\n");
    printf("║ 3. Show Parser                                            ║\n");
    printf("║ 4. Show CPU                                               ║\n");
    printf("║ 5. Show Entire CPU (with Code, Stack, and Extra Segments) ║\n");
    printf("║ 6. Exit                                                   ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n");
    printf("\n");
    int ipt=input(1,6);
    
    switch(ipt){
        case 1:
            {Segment *DS=hashmap_get(cpu->memory_handler->allocated,"DS");
            if (DS){
               printf("Variables already allocated.\n"); 
               menu3(cpu,parser);
            }else{
            allocate_variables(cpu,parser->data_instructions,parser->data_count);
            }
            break;
            }
        case 2:
            {
            if (resolve_constants(parser)==0) {
                 printf("Error resolving constants in parser for code allocation.\n");
                 menu3(cpu,parser);
                 break;
            }
            Segment *CS=hashmap_get(cpu->memory_handler->allocated,"CS");
            if (CS){
               printf("Code segment already allocated.\n"); 
               menu3(cpu,parser);
            }else{
                allocate_code_segment(cpu,parser->code_instructions,parser->code_count);
            }
            break;
            }
        case 3:
            parser_show(parser);
            printf("\n");
            break;

        case 4:
            print_cpu(cpu);
            printf("\n");
            break;

        case 5:
            print_entire_cpu(cpu);
            printf("\n");
            break;

        case 6:
            printf("Exiting simulator. Goodbye!\n\n");
            cpu_destroy(cpu);
            free_parser_result(parser);
            usleep(1300000);
            exit(0); 
    }
}

void menucs(CPU* cpu, ParserResult* parser){
    int c;
    while ((c = getchar()) != '\n' && c != EOF); 
    printf("\n-------[Press ENTER to continue...]-------\n\n");
    while (getchar() != '\n');
    printf("\033[2J\033[H"); 
    printf("\nSelect an option (1-5): \n");
    printf("╔═══════════════════════════════════════════════════════════╗\n");
    printf("║                    CPU SIMULATOR MENU                     ║\n");
    printf("╠═══════════════════════════════════════════════════════════╣\n");
    printf("║ 1. Allocate Code Segment                                  ║\n");
    printf("║ 2. Show Parser                                            ║\n");
    printf("║ 3. Show CPU                                               ║\n");
    printf("║ 4. Show Entire CPU (with Code, Stack, and Extra Segments) ║\n");
    printf("║ 5. Exit                                                   ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n");
    printf("\n");
    int ipt=input(1,5);
    
    switch(ipt){
        case 1:
            {
            if (resolve_constants(parser)==0) {
                 printf("Error resolving constants in parser for code allocation.\n");
                 menu3(cpu,parser);
                 break;
            }
            Segment *CS=hashmap_get(cpu->memory_handler->allocated,"CS");
            if (CS){
               printf("Code segment already allocated.\n"); 
               menu3(cpu,parser);
            }else{
                allocate_code_segment(cpu,parser->code_instructions,parser->code_count);
            }
            break;
            }
            
        case 2:
            parser_show(parser);
            printf("\n");
            break;

        case 3:
            print_cpu(cpu);
            printf("\n");
            break;

        case 4:
            print_entire_cpu(cpu);
            printf("\n");
            break;

        case 5:
            printf("Exiting simulator. Goodbye!\n\n");
            cpu_destroy(cpu);
            free_parser_result(parser);
            usleep(1300000);
            exit(0); 
    }
}

void menuds(CPU* cpu, ParserResult* parser){
    int c;
    while ((c = getchar()) != '\n' && c != EOF); 
    printf("\n-------[Press ENTER to continue...]-------\n\n");
    while (getchar() != '\n');
    printf("\033[2J\033[H"); 
    printf("\nSelect an option (1-5): \n");
    printf("╔═══════════════════════════════════════════════════════════╗\n");
    printf("║                    CPU SIMULATOR MENU                     ║\n");
    printf("╠═══════════════════════════════════════════════════════════╣\n");
    printf("║ 1. Allocate Data Segment                                  ║\n");
    printf("║ 2. Show Parser                                            ║\n");
    printf("║ 3. Show CPU                                               ║\n");
    printf("║ 4. Show Entire CPU (with Code, Stack, and Extra Segments) ║\n");
    printf("║ 5. Exit                                                   ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n");
    printf("\n");
    int ipt=input(1,5);
    
    switch(ipt){
        case 1:
            {Segment *DS=hashmap_get(cpu->memory_handler->allocated,"DS");
            if (DS){
               printf("Variables already allocated.\n"); 
               menu3(cpu,parser);
            }else{
            allocate_variables(cpu,parser->data_instructions,parser->data_count);
            }
            break;
            }
            
        case 2:
            parser_show(parser);
            printf("\n");
            break;

        case 3:
            print_cpu(cpu);
            printf("\n");
            break;

        case 4:
            print_entire_cpu(cpu);
            printf("\n");
            break;

        case 5:
            printf("Exiting simulator. Goodbye!\n\n");
            cpu_destroy(cpu);
            free_parser_result(parser);
            usleep(1300000);
            exit(0); 
    }
}



int menu4(CPU* cpu, ParserResult* parser){
    int c;
    while ((c = getchar()) != '\n' && c != EOF); 
    printf("\n-------[Press ENTER to continue...]-------\n\n");
    while (getchar() != '\n');
    printf("\033[2J\033[H"); 
    printf("\nSelect an option (1-5): \n");
    printf("╔═══════════════════════════════════════════════════════════╗\n");
    printf("║                    CPU SIMULATOR MENU                     ║\n");
    printf("╠═══════════════════════════════════════════════════════════╣\n");
    printf("║ 1. RUN PROGRAM                                            ║\n");
    printf("║ 2. Show Parser                                            ║\n");
    printf("║ 3. Show CPU                                               ║\n");
    printf("║ 4. Show Entire CPU (with Code, Stack, and Extra Segments) ║\n");
    printf("║ 5. Exit                                                   ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n");
    printf("\n");
    int ipt=input(1,5);

    switch(ipt){
        case 1:
            usleep(1300000);
            printf("\033[2J\033[H");
            if(run_program_preview(cpu)==0){
                printf("Error Running Program, Aborted.\n");
                return 0;

            }
            return 1;
            break;
            
            
        case 2:
            parser_show(parser);
            printf("\n");
            break;

        case 3:
            print_cpu(cpu);
            printf("\n");
            break;

        case 4:
            print_entire_cpu(cpu);
            printf("\n");
            break;

        case 5:
            printf("Exiting simulator. Goodbye!\n\n");
            cpu_destroy(cpu);
            free_parser_result(parser);
            usleep(1300000);
            exit(0); 
    }
    return 1;

}

int menu5(CPU* cpu, ParserResult* parser){
    int c;
    while ((c = getchar()) != '\n' && c != EOF); 
    printf("\n-------[Press ENTER to continue...]-------\n\n");
    while (getchar() != '\n');
    printf("\033[2J\033[H"); 
    printf("\nSelect an option (1-4): \n");
    printf("╔═══════════════════════════════════════════════════════════╗\n");
    printf("║                    CPU SIMULATOR MENU                     ║\n");
    printf("╠═══════════════════════════════════════════════════════════╣\n");
    printf("║ 1. Show Parser                                            ║\n");
    printf("║ 2. Show CPU                                               ║\n");
    printf("║ 3. Show Entire CPU (with Code, Stack, and Extra Segments) ║\n");
    printf("║ 4. Exit                                                   ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n");
    printf("\n");
    int ipt=input(1,4);

    switch(ipt){
            
        case 1:
            parser_show(parser);
            printf("\n");
            break;

        case 2:
            print_cpu(cpu);
            printf("\n");
            break;

        case 3:
            print_entire_cpu(cpu);
            printf("\n");
            break;

        case 4:
            printf("\nThank you for using the CPU Simulator!\n");
            printf("Hope to see you again soon. Goodbye! \n");
            cpu_destroy(cpu);
            free_parser_result(parser);
            usleep(1300000);
            exit(0); 
    }
    return 1;

}




int main(){

    while (1){
    printf("\nWelcome to the CPU simulator !\n");
    usleep(200000);

    CPU *cpu=menu1();

    ParserResult* parser=menu2(cpu);

    while (!parser){
        parser=menu2(cpu);
    }

    while(hashmap_get(cpu->memory_handler->allocated,"DS")==NULL &&
    hashmap_get(cpu->memory_handler->allocated,"CS")==NULL){
        menu3(cpu,parser);
    }

    while(hashmap_get(cpu->memory_handler->allocated,"DS")==NULL) {
    menuds(cpu,parser);
    }

    while(hashmap_get(cpu->memory_handler->allocated,"CS")==NULL) {
    menucs(cpu,parser);
    }

    if (menu4(cpu,parser)){
        while(1){
            menu5(cpu,parser);
        }
    }

    if (cpu) cpu_destroy(cpu);
    if (parser) free_parser_result(parser);

    }

    return 0;

}
    


    

















