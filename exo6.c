#include "exo6.h"


char *trim(char *str) {
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
                //free(input);
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
    if (!result) return 0;
    for(int i = 0; i<result->code_count;i++){
        Instruction * inst = result->code_instructions[i];
        if(inst->operand2){
            search_and_replace(&(inst->operand2),result->memory_locations);
            search_and_replace(&(inst->operand1),result->memory_locations);
        }else{
            search_and_replace(&(inst->operand1),result->labels);
        } 
        
    }
    return 1;
}

void allocate_code_segment(CPU *cpu, Instruction **code_instructions, int code_count){
    int start = cpu->memory_handler->free_list->start;
    create_segment(cpu->memory_handler,"CS",start,code_count);
    for (int i=0;i<code_count;i++){
        store(cpu->memory_handler,"CS",i,code_instructions[i]);
    }
    int * reg_ip = (int*)hashmap_get(cpu->context,"IP");
    if(*reg_ip) *reg_ip = 0;
}

int handle_instruction(CPU *cpu, Instruction *instr, void *src, void *dest){
    char * mnemonic = instr->mnemonic;
    if(strcmp(mnemonic,"MOV")==0){
        handle_MOV(cpu,src,dest);
        return 1;
    }
    if(strcmp(mnemonic,"ADD")==0){
        *((int*)dest) = *((int*)dest) + *((int*)src);
        return 1;
    }
    if(strcmp(mnemonic,"CMP")==0){
        int res=(*(int *)dest) - (*(int *)src);
        if (res==0) {
            int * reg_zf = (int *)hashmap_get(cpu->context,"ZF");
            if(reg_zf) *reg_zf = 1;
        }
        if (res<0){
            int * reg_sf = (int *)hashmap_get(cpu->context,"SF");
            if(reg_sf) *reg_sf = 1;
        }
        return 1;
    }
    if(strcmp(mnemonic,"JMP")==0){
        int * reg_ip = (int *)hashmap_get(cpu->context,"IP");
        int value;
        sscanf(instr->operand1,"%d",&value);
        *reg_ip = value;
        return 1;
    }
    if(strcmp(mnemonic,"JZ")==0){
        int * reg_zf = (int *)hashmap_get(cpu->context,"ZF");
        if(*reg_zf==1){
            int * reg_ip = (int *)hashmap_get(cpu->context,"IP");
            int value;
            sscanf(instr->operand1,"%d",&value);
            *reg_ip = value;
        }
        return 1;
    }
    if(strcmp(mnemonic,"JNZ")==0){
        int * reg_zf = (int *)hashmap_get(cpu->context,"ZF");
        if(*reg_zf==0){
            int * reg_ip = (int *)hashmap_get(cpu->context,"IP");
            int value;
            sscanf(instr->operand1,"%d",&value);
            *reg_ip = value;
        }
        return 1;
    }
    if(strcmp(mnemonic,"HALT")==0){
        Segment * cs = (Segment *)hashmap_get(cpu->memory_handler->allocated,"CS");
        if (!cs) return 0;
        int * reg_ip = (int *)hashmap_get(cpu->context,"IP");
        *reg_ip=cs->size-1;
        return 1;
    }
    if(strcmp(mnemonic,"PUSH")==0){
        int *reg;
        if (src) reg=hashmap_get(cpu->context,src);
        else  reg=hashmap_get(cpu->context,"AX");
        int p=push_value(cpu,*reg);
        if (p!=-1) return 1;
    }

    if(strcmp(mnemonic,"POP")==0){
        int *reg;
        if (dest) reg=hashmap_get(cpu->context,dest);
        else  reg=hashmap_get(cpu->context,"AX");
        int p=pop_value(cpu,reg);
        if (p!=-1) return 1;
    }

    if(strcmp(mnemonic,"ALLOC")==0){
        int succ=alloc_es_segment(cpu);
        return succ;
    }

    if(strcmp(mnemonic,"FREE")==0){
        int succ=free_es_segment(cpu);
        return succ;
    }

    return 0;
}

int execute_instruction(CPU *cpu, Instruction *instr){

    if (strcmp(instr->mnemonic, "JMP") == 0 || strcmp(instr->mnemonic, "JZ") == 0 ||
     strcmp(instr->mnemonic, "JNZ") == 0){
        return handle_instruction(cpu,instr,resolve_addressing(cpu,instr->operand1),NULL);
    }

    if (strcmp(instr->mnemonic, "PUSH") == 0) {
        return handle_instruction(cpu,instr,resolve_addressing(cpu,instr->operand1),NULL);
    }   
    
    if (strcmp(instr->mnemonic, "POP") == 0){
        return handle_instruction(cpu,instr,NULL,resolve_addressing(cpu,instr->operand1));
    }

    return handle_instruction(cpu,instr,resolve_addressing(cpu,instr->operand2),resolve_addressing(cpu,instr->operand1));

}

Instruction *fetch_next_instruction(CPU *cpu){
    Segment *cs=hashmap_get(cpu->memory_handler->allocated,"CS");
    int* ip =hashmap_get(cpu->context,"IP");
    if (ip==NULL || cs==NULL || *ip<cs->start || *ip<cs->start+cs->size) return NULL;
    Instruction* inst =(Instruction *) load(cpu->memory_handler,"CS",*ip);
    (ip)++;
    return inst;
}

int run_program(CPU *cpu){  //memory handler deja rempli
    printf("CPU INITIAL STATE : \n");
    print_data_segment(cpu);
    print_hashmap_int(cpu->context);

    Segment *ds=hashmap_get(cpu->memory_handler->allocated,"DS");
    if (!ds) return 0;

    Instruction *courant=fetch_next_instruction(cpu);
    printf("pas d'instructions\n");
    while (courant){
       
        printf("PRESS \"ENTER\" TO EXECUTE THE NEXT INSTRUCTION : ");
        print_instruction(courant);
        printf("OR \"q\" to QUIT EXECUTION\n ");
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