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
            int value = (int)(long)values->table[i].value;

            // Find potential substring match
            char *substr = strstr(input, key);
            if (substr) {
                // Construct replacement buffer
                char replacement[64];
                snprintf(replacement, sizeof(replacement), "%d", value);

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
                free(input);
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
            search_and_replace(inst->operand2,result->memory_locations);
        } else {
            search_and_replace(inst->operand1,result->labels);
        }
    }
    return 1;
}

void allocate_code_segment(CPU *cpu, Instruction **code_instructions, int code_count){
    int start = cpu->memory_handler->free_list->start;
    create_segment(cpu->memory_handler,"CS",start,code_count);
    for (int i=0;i<code_count;i++){
        cpu->memory_handler->memory[start+i] = code_instructions[i];
    }
    // int * reg_ip = (int*)hashmap_get(cpu->context,"IP");
    // if(reg_ip) *reg_ip = 0;
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
    return 0;
}

int execute_instruction(CPU *cpu, Instruction *instr){
    void *dest = resolve_addressing(cpu,instr->operand1);
    void *src = NULL;
    if(instr->operand2){
        src = resolve_addressing(cpu,instr->operand2);
    }
    return handle_instruction(cpu,instr,src,dest);
}