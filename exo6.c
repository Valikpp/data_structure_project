#include "exo6.h"


char *trim(char *str) {
    /*
        Trim Whitespace from String:
        Removes leading and trailing whitespace characters (spaces, tabs, newlines, carriage returns) from the given string.
        
        Input: 
            char *str - a modifiable C string that may contain leading or trailing whitespace
        Output: 
            char * - pointer to the first non-whitespace character of the modified input string
                    (the original string is modified in-place and returned)
    */
    while (*str == ' ' || *str == '\t' || *str == '\n' || *str == '\r') {
        str++;
    }

    char *end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')) {
        *end = '\0';
        end--;
    }
    return str;
}

int search_and_replace(char **str, HashMap *values) {
    /*
        String interpolation using a context Hashmap:
        For every key of HashMap presented in str the function replaces key by value

        Input: 
            char **str -- double pointer to string (necessary to work in-place)
            HashMap * values -- HashMap of context information
        Output: 
            1 in case of successful replace
            0 -- Hashmap's keys are not present in str
    */
    
    if (!str || !*str || !values) return 0;

    int replaced = 0;
    char *input = *str;

    // Iterate through all keys in the hashmap
    for (int i = 0; i < values->size; i++) {
        if (values->table[i].key && values->table[i].key != (void *)-1) {
            char *key = values->table[i].key;
            int* value = (int*)(long)values->table[i].value;  //corrigé erreur int -> int* 

            // Find potential substring match
            char *substr = strstr(input, key);
            if (substr) {
                // Construct replacement buffer
                char replacement[64];
                snprintf(replacement, sizeof(replacement), "%d", *value);

                // Calculate lengths
                int key_len = strlen(key);
                int repl_len = strlen(replacement);
                int remain_len = strlen(substr + key_len);

                // Create new string
                char *new_str = (char *)malloc(strlen(input) - key_len + repl_len + 1);
                strncpy(new_str, input, substr - input);
                new_str[substr - input] = '\0';
                strcat(new_str, replacement);
                strcat(new_str, substr + key_len);

                // Free and update original string
                free(input);                                  //enlevé le free
                *str = new_str;
                input = new_str;

                replaced = 1;
            }
        }
    }

    // Trim the final string
    if (replaced) {
        char *trimmed = trim(input);
        if (trimmed != input) {
            memmove(input, trimmed, strlen(trimmed) + 1);
        }
    }

    return replaced;
}

int resolve_constants(ParserResult * result){
    /*
        .CODE instruction's operand treatment
        Replaces variables with their address in the data segment and labels with their address in the code.
        
        Input: 
            ParserResult * result -- result of pseudo-assembler parse
        Output: 
            1 in case of successful replace for all Instructions
            0 in other cases
    */
    if (!result) return 0;
    for(int i = 0; i<result->code_count;i++){
        Instruction * inst = result->code_instructions[i];
        if(inst->operand2){ // Current instruction own an operand2
            search_and_replace(&(inst->operand2),result->memory_locations);
            search_and_replace(&(inst->operand1),result->memory_locations);
        }else{ // Only 1 operand 
            if(inst->operand1){
                int succ=search_and_replace(&(inst->operand1),result->labels);
                // No substitution has occurred, and also the mnemonic is not a push or pop operation
                if (!succ && strcmp(inst->mnemonic,"PUSH") && strcmp(inst->mnemonic,"POP")) {
                    printf("Error resolve_constants : invalid address with mnemonic %s \n",inst->mnemonic);
                    return 0;
                }
            }
        } 
        
    }
    return 1;
}

void allocate_code_segment(CPU *cpu, Instruction **code_instructions, int code_count){
    /*
        Allocation of segment for .CODE instructions
        Allocates a CS segment at the first free place and stores all .CODE instructions in memory
        Sets a iteration pointer IP register to 0 (first line of code)
        
        Input: 
            CPU *cpu -- initialized CPU
            Instruction **code_instructions -- list of .CODE instructions AFTER CONSTANTS RESOLVING 
            int code_count -- number of .CODE instructions
    */
    int start = find_free_address_strategy(cpu->memory_handler,code_count,1);
    create_segment(cpu->memory_handler,"CS",start,code_count);
    for (int i=0;i<code_count;i++){
        store(cpu->memory_handler,"CS",i,code_instructions[i]);
    }
    int * reg_ip = (int*)hashmap_get(cpu->context,"IP");
    if(*reg_ip) *reg_ip = 0;
}

int handle_instruction(CPU *cpu, Instruction *instr, void *src, void *dest){
    /*
        Treatment of instruction's operations 
        According the mnemonic execute an instruction

        Input: 
            CPU *cpu -- initialized CPU
            Instruction *instr -- .CODE instruction AFTER CONSTANTS RESOLVING 
            void * src -- pointer to source value for operation (Responds at "which value use?")
            void * dest -- pointer to destination used to store the result of operation (Responds at "Where store the result?")
        Output:
            1 in case of successful execution of instruction
            0 in case of wrong mnemonic or execution error    
*/
    char * mnemonic = instr->mnemonic;
    //Already defined
    if(strcmp(mnemonic,"MOV")==0){
        handle_MOV(cpu,src,dest);
        return 1;
    }

    /*
        Addition operation: adds value of source to value of dest
        Stores result at dest
    */
    if(strcmp(mnemonic,"ADD")==0){
        *((int*)dest) = *((int*)dest) + *((int*)src);
        return 1;
    }

    /*
        Comparison operation: Comparing the value of src and dest
        Sets ZF to 1 at case of equality
        Sets SF to 1 if src is bigger that result
    */
    if(strcmp(mnemonic,"CMP")==0){
        int res=(*(int *)dest) - (*(int *)src);
        int * zf = (int *)hashmap_get(cpu->context,"ZF");
        int * sf = (int *)hashmap_get(cpu->context,"SF");
        if (res==0) {
            if(zf) *zf = 1;

        }
        if (res<0){
            if(sf) *sf = 1;
            if(zf) *zf = 0;
        }
        return 1;
    }
    /*
        Operate to move the executor (IP) to the specified line
    */
    if(strcmp(mnemonic,"JMP")==0){
        int * reg_ip = (int *)hashmap_get(cpu->context,"IP");
        *reg_ip =*(int *)src;
        return 1;
    }
    /*
        Operate to move the executor (IP) to the specified line in case than previous instruction was terminated with result 0
    */
    if(strcmp(mnemonic,"JZ")==0){
        int * reg_zf = (int *)hashmap_get(cpu->context,"ZF");
        if(*reg_zf==1){
            int * reg_ip = (int *)hashmap_get(cpu->context,"IP");
            *reg_ip = *(int *)src;
        }
        return 1;
    }
    /*
        Operate to move the executor (IP) to the specified line in case than previous instruction was terminated with non-zero result 
    */
    if(strcmp(mnemonic,"JNZ")==0){
        int * reg_zf = (int *)hashmap_get(cpu->context,"ZF");
        if(*reg_zf==0){
            int * reg_ip = (int *)hashmap_get(cpu->context,"IP");
            *reg_ip = *(int *)src;
        }
        return 1;
    }
    /*
        Stops code execution by moving the executor to the end of CS segment
    */
    if(strcmp(mnemonic,"HALT")==0){
        Segment * cs = (Segment *)hashmap_get(cpu->memory_handler->allocated,"CS");
        if (!cs) return 0;
        int * reg_ip = (int *)hashmap_get(cpu->context,"IP");
        *reg_ip=cs->size;
        return 1;
    }
    /*
        Pushes value of src in stack (value of AX if src undefined)
    */
    if(strcmp(mnemonic,"PUSH")==0){
        int *reg;
        if (src) reg=src;
        else reg=hashmap_get(cpu->context,"AX");
        int p=push_value(cpu,*reg);
        if (p!=-1) return 1;
    }
    /*
        Removes value from stack and stores value in dest (in AX if dest undefined)
    */
    if(strcmp(mnemonic,"POP")==0){
        int *reg;
        if (dest) reg=dest;
        else reg=hashmap_get(cpu->context,"AX");
        int p=pop_value(cpu,reg);
        if (p!=-1) return 1;
    }
    /*
        Allocation of ES segment
    */
    if(strcmp(mnemonic,"ALLOC")==0){
        int succ=alloc_es_segment(cpu);
        return succ;
    }
    /*
        Clearing of ES segment
    */
    if(strcmp(mnemonic,"FREE")==0){
        int succ=free_es_segment(cpu);
        return succ;
    }

    return 0;
}

int execute_instruction(CPU *cpu, Instruction *instr){
    /*
        Executing of instruction  
        Apply a correct way to treat the instruction according the type of mnemonic

        Input: 
            CPU *cpu -- initialized CPU
            Instruction *instr -- .CODE instruction AFTER CONSTANTS RESOLVING 
        Output:
            1 in case of successful execution
            0 in other cases
    */
    
    // Jump or conditional jump instruction take operand1 as a source
    if (strcmp(instr->mnemonic, "JMP") == 0 || strcmp(instr->mnemonic, "JZ") == 0 ||
     strcmp(instr->mnemonic, "JNZ") == 0){
        return handle_instruction(cpu,instr,resolve_addressing(cpu,instr->operand1),NULL);
    }
    // Push takes operand1 as a source
    if (strcmp(instr->mnemonic, "PUSH") == 0) {
        return handle_instruction(cpu,instr,resolve_addressing(cpu,instr->operand1),NULL);
    }   
    // Pop takes operand1 as a destination
    if (strcmp(instr->mnemonic, "POP") == 0){
        return handle_instruction(cpu,instr,NULL,resolve_addressing(cpu,instr->operand1));
    }
    // Operand2 - source, operand1 - dest
    return handle_instruction(cpu,instr,resolve_addressing(cpu,instr->operand2),resolve_addressing(cpu,instr->operand1));

}

Instruction *fetch_next_instruction(CPU *cpu){
    /*
        Loading of next instruction
        Loads a next instruction to execute and increments executor pointer IP

        Input: 
            CPU *cpu -- initialized CPU
        Output:
            Instruction * inst -- instruction to execute
            NULL -- no executor/no code segment/no more instructions
    */
    Segment *cs=hashmap_get(cpu->memory_handler->allocated,"CS");
    int* ip =hashmap_get(cpu->context,"IP");
    if (ip==NULL || cs==NULL || *ip>=cs->size) return NULL;
    Instruction* inst =(Instruction *) load(cpu->memory_handler,"CS",*ip);
    (*ip)++;
    return inst;
}

int run_program(CPU *cpu){  //memory handler deja rempli
    printf("CPU INITIAL STATE : \n");
    print_data_segment(cpu);
    print_hashmap_int(cpu->context);

    Segment *ds=hashmap_get(cpu->memory_handler->allocated,"DS");
    if (!ds) return 0;

    Instruction *courant=fetch_next_instruction(cpu);
    while (courant){
       
        printf("PRESS \"ENTER\" TO EXECUTE THE NEXT INSTRUCTION : ");
        print_instruction(courant);
        printf("OR \"Q\" to QUIT EXECUTION\n ");
        char val;
        scanf("%c",&val);
        if (val=='\n'){
            int e=execute_instruction(cpu,courant);
            if (!e) return 0;
            courant=fetch_next_instruction(cpu);
        }
        else if (val=='q' || val=='Q'){
            break; //je remet ip a 0?
        }
    }

    printf("CPU FINAL STATE : \n");
    print_data_segment(cpu);
    print_hashmap_int(cpu->context);
    printf("\n");
    return 1;
}

int run_program_preview(CPU *cpu) { 
    printf("CPU INITIAL STATE : \n");
    print_cpu(cpu);

    Segment *ds = hashmap_get(cpu->memory_handler->allocated, "DS");
    if (!ds) return 0;
    int c;
    while ((c = getchar()) != '\n' && c != EOF); 
    Instruction *courant = fetch_next_instruction(cpu);
    while (courant) {
        printf("PRESS \"ENTER\" TO EXECUTE THE NEXT INSTRUCTION : [ ");
        print_instruction(courant);
        printf("] OR \"Q\" to QUIT EXECUTION\n ");

        char input[10];
        if (fgets(input, sizeof(input), stdin) == NULL) break;

        if (input[0] == '\n') {
            printf("\033[2J\033[H");
            int e = execute_instruction(cpu, courant);
            if (!e) return 0;
            courant = fetch_next_instruction(cpu);
            print_cpu(cpu);
        } else if (input[0] == 'q' || input[0] == 'Q') {
            break;
        }
    }

    return 1;
}

    

void print_entire_cpu(CPU *cpu){
    if (!cpu) {
        printf("Error print_entire_cpu : No CPU to print");
        return;
    }
    printf("\n---------- ENTIRE CPU: ----------\n\n");
    Segment *SS= hashmap_get(cpu->memory_handler->allocated,"SS");
    if (SS){
    printf("*** STACK SEGMENT: [");
    
    for(int i = 0; i < SS->size; i++){
        void *value = load(cpu->memory_handler, "SS", i);
        if (value) {
            printf("%d", *(int*)value);
        } else {
            printf("_");
        }

        if (i < SS->size - 1) {
            printf(",");
        }
    }

    printf("] *** \n\n");
    }

    Segment *DS= hashmap_get(cpu->memory_handler->allocated,"DS");
    if (DS){
    printf("*** DATA SEGMENT: [");
    
    for(int i = 0; i < DS->size; i++){
        void *value = load(cpu->memory_handler, "DS", i);
        if (value) {
            printf("%d", *(int*)value);
        } else {
            printf("_");
        }

        if (i < DS->size - 1) {
            printf(",");
        }
    }

    printf("] *** \n\n");
    }

    Segment *CS= hashmap_get(cpu->memory_handler->allocated,"CS");
    if (CS){
    printf("*** CODE SEGMENT: [");
    
    for(int i = 0; i < CS->size; i++){
        void *value = load(cpu->memory_handler, "CS", i);
        if (value) {
            print_instruction((Instruction *)value);
        } else {
            printf("_");
        }

        if (i < CS->size - 1) {
            printf(",");
        }
    }

    printf("] *** \n\n");
    }

    Segment *ES= hashmap_get(cpu->memory_handler->allocated,"ES");
    if (ES){
    printf("*** EXTRA SEGMENT: [");
    
    for(int i = 0; i < ES->size; i++){
        void *value = load(cpu->memory_handler, "ES", i);
        if (value) {
            printf("%d", *(int*)value);
        } else {
            printf("_");
        }

        if (i < ES->size - 1) {
            printf(",");
        }
    }

    printf("] *** \n\n");
    }

    printf("*** context :\n");
    print_hashmap_int(cpu->context);
    printf("***");
    printf("\n---------- END OF CPU ----------\n\n");
    
}

void print_cpu(CPU* cpu){
    if (!cpu) {
        printf("Error print_cpu : No CPU to print");
        return;
    }
    printf("\n---------- CPU CONTENT: ----------\n\n");

    Segment *DS= hashmap_get(cpu->memory_handler->allocated,"DS");
    if (DS){
    printf("*** DATA SEGMENT: [");
    
    for(int i = 0; i < DS->size; i++){
        void *value = load(cpu->memory_handler, "DS", i);
        if (value) {
            printf("%d", *(int*)value);
        } else {
            printf("_");
        }

        if (i < DS->size - 1) {
            printf(",");
        }
    }

    printf("] *** \n\n");
    }

    printf("*** context :\n");
    print_hashmap_int(cpu->context);
    printf("***");

    printf("\n---------- END OF CPU ----------\n\n");

}



  

