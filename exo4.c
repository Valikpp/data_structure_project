#include "exo4.h"

CPU * cpu_init(int memory_size){
    CPU * cpu = (CPU*)malloc(sizeof(CPU));
    cpu->memory_handler = memory_init(memory_size);
    cpu->context = hashmap_create();
    hashmap_insert(cpu->context, "AX", int_to_point(0));
    hashmap_insert(cpu->context, "BX", int_to_point(0));
    hashmap_insert(cpu->context, "CX", int_to_point(0));
    hashmap_insert(cpu->context, "DX", int_to_point(0));
    hashmap_insert(cpu->context, "IP", int_to_point(0));
    hashmap_insert(cpu->context, "ZF", int_to_point(0));
    hashmap_insert(cpu->context, "SF", int_to_point(0));
    cpu->constant_pool = hashmap_create();
    return cpu;
}

void cpu_destroy(CPU *cpu){
    free_memory_handler(cpu->memory_handler);
    hashmap_destroy(cpu->context);
    hashmap_destroy(cpu->constant_pool);
    free(cpu);
}

void* store(MemoryHandler *handler, const char *segment_name, int pos, void *data){
    Segment *seg=(Segment *)hashmap_get(handler->allocated,segment_name);
    if (!seg || (pos >= seg->size)) return NULL;
  
    int final_pos = seg->start + pos;
    //si l'espace est deja occupe on free et on remplace par data
    if(handler->memory[final_pos]){
        free(handler->memory[final_pos]);
    }
    handler->memory[final_pos] = data;
    return data;
}

void *load(MemoryHandler *handler, const char *segment_name, int pos){
    Segment *seg=(Segment *)hashmap_get(handler->allocated,segment_name);
    if (!seg || (pos >= seg->size)) return NULL;
    int final_pos = seg->start + pos;

    return (handler->memory[final_pos]);
}

void allocate_variables(CPU *cpu, Instruction** data_instructions,int data_count){
    // nb_data_occ -- external variable qui contient la taille de l'espace necessaire au stockage des variables  
    create_segment(cpu->memory_handler,"DS",cpu->memory_handler->free_list->start,nb_data_occ);

    //indice dans la memoire de CPU
    int pos=0;
    for(int i = 0; i<data_count;i++){
        char * operand2 = data_instructions[i]->operand2;
        //indice general sur la chaine
        int k = 0;
        char buffer[20];
        //indice sur buffer 
        int buff_ind = 0;
        //printf("--------Instruction---------:\n");
        instruction_show(data_instructions[i]);
        //printf("--------Parsed values---------:\n");
        while (operand2[k]!='\0')
        {
            if (operand2[k]==','){
                int value;
                sscanf(buffer," %d ",&value);
                store(cpu->memory_handler,"DS",pos,int_to_point(value));
                //printf("memory[%d] = %d \n",pos,value);
                // valeur de buffer est recureperee, on remet remet a vide (en rangeant l'indice)
                strcpy(buffer, "                   ");
                buff_ind = 0;
                pos++;
            } else {
                buffer[buff_ind] = operand2[k];
                buff_ind++;
            }
            k++;
        }
        int value;
        sscanf(buffer," %d ",&value);
        // printf("memory[%d] = %d ",pos,value);
        // printf("\n\n");
        store(cpu->memory_handler,"DS",pos,int_to_point(value));
        pos++;
    }
}

void print_data_segment(CPU * cpu){
    Segment * segment = (Segment*)hashmap_get(cpu->memory_handler->allocated,"DS");
    if(!segment) return;
    printf("========= Content of memory segment <DS> =========\nDS = [");
    for(int i=0;i<segment->size;i++){
        if (cpu->memory_handler->memory[i]){
            printf("%d,",*(int*)cpu->memory_handler->memory[i]);
        } else {
            printf("_,");
        }
    }
    printf("]\n");
    printf("========= END of memory segment <DS> =========\n");
}






void preview_allocate_variables(CPU *cpu, Instruction** data_instructions,int data_count){
    // nb_data_occ -- external variable qui contient la taille de l'espace necessaire au stockage des variables  
    create_segment(cpu->memory_handler,"DS",cpu->memory_handler->free_list->start,nb_data_occ);

    //indice dans la memoire de CPU
    int pos=0;
    for(int i = 0; i<data_count;i++){
        char * operand2 = data_instructions[i]->operand2;
        //indice general sur la chaine
        int k = 0;
        char buffer[20];
        //indice sur buffer 
        int buff_ind = 0;
        printf("--------Instruction---------:\n");
        instruction_show(data_instructions[i]);
        printf("--------Parsed values---------:\n");
        while (operand2[k]!='\0')
        {
            if (operand2[k]==','){
                int value;
                sscanf(buffer," %d ",&value);
                //store(cpu->memory_handler,"DS",pos,int_to_point(value));
                printf("memory[%d] = %d \n",pos,value);
                // valeur de buffer est recuperee, on remet a vide (en rangeant l'indice)
                strcpy(buffer, "                   ");
                buff_ind = 0;
                pos++;
            } else {
                buffer[buff_ind] = operand2[k];
                buff_ind++;
            }
            k++;
        }
        int value;
        sscanf(buffer," %d ",&value);
        printf("memory[%d] = %d ",pos,value);
        printf("\n\n");
        //store(cpu->memory_handler,"DS",pos,int_to_point(value));
        pos++;
    }
}


int matches(const char *pattern, const char *string) {
    regex_t regex;
    int result = regcomp(&regex, pattern, REG_EXTENDED);
    if (result) {
        fprintf(stderr, "Regex compilation failed for pattern: %s\n", pattern);
        return 0;
    }
    result = regexec(&regex, string, 0, NULL, 0);
    regfree(&regex);
    return result == 0;
}

void * immediate_addressing(CPU * cpu, const char * operand){
    char * pattern = "^[0-9]+$";
    if (!matches(pattern,operand)){
        return NULL;
    }
    int * result = (int *)hashmap_get(cpu->constant_pool,operand);
    if (result){
        return (void*)result;
    }
    int value;
    sscanf(operand,"%d",&value);
    result = int_to_point(value);
    hashmap_insert(cpu->constant_pool,operand,result);
    return (void*)result;  
}

void *register_addressing(CPU * cpu, const char*operand){
    char * pattern = "^[A-D]X$";
    if (!matches(pattern,operand)){
        return NULL;
    }
    int *result=(int *)hashmap_get(cpu->context,operand);
    if (!result) return NULL;

    return (void *) result;
}

void *memory_direct_addressing(CPU * cpu, const char*operand){
    char * pattern = "^\\[[0-9]+\\]$";
    if (!matches(pattern,operand)){
        return NULL;
    }
    int value;
    sscanf(operand,"[%d]",&value);
    if (value>=cpu->memory_handler->total_size){
        return NULL;
    }
    return cpu->memory_handler->memory[value];
}

void * register_indirect_addressing(CPU * cpu, const char*operand){
    char * pattern = "^\\[[A-D]X\\]$";
    if (!matches(pattern,operand)){
        return NULL;
    }
    char * regist = (char*)malloc(sizeof(char)*3);
    strncpy(regist,operand+1,2);
    regist[2] = '\0';
    int *result=(int *)hashmap_get(cpu->context,regist);
    free(regist);
    if (!result) return NULL;
    if ((*(int*)result)>=cpu->memory_handler->total_size){
        return NULL;
    }
    return cpu->memory_handler->memory[*(int*)result];
}

void handle_MOV(CPU * cpu, void * src, void *dest){
    *((int*)dest) = *((int*)src);
}

CPU *setup_test_environment() {
    // Initialiser le CPU
    CPU *cpu = cpu_init(1024);
    if (!cpu) {
        printf("Error: CPU initialization failed\n");
        return NULL;
    }

    // Initialiser les registres avec des valeurs spécifiques
    int *ax = (int *)hashmap_get(cpu->context, "AX");
    int *bx = (int *)hashmap_get(cpu->context, "BX");
    int *cx = (int *)hashmap_get(cpu->context, "CX");
    int *dx = (int *)hashmap_get(cpu->context, "DX");

    *ax = 3;
    *bx = 6;
    *cx = 100;
    *dx = 0;

    // Créer et initialiser le segment de données
    if (!hashmap_get(cpu->memory_handler->allocated, "DS")) {
        create_segment(cpu->memory_handler, "DS", 0, 20);

        // Initialiser le segment de données avec des valeurs de test
        for (int i = 0; i < 10; i++) {
            int *value = (int *)malloc(sizeof(int));
            *value = i * 10 + 5;  // Valeurs 5, 15, 25, 35...
            store(cpu->memory_handler, "DS", i, value);
        }
    }

    printf("Test environment initialized.\n");
    return cpu;
}

void *resolve_addressing(CPU *cpu, const char *operand){
    void *pt=immediate_addressing(cpu,operand);
    if (!pt) pt=register_addressing(cpu, operand);
    if (!pt) pt=memory_direct_addressing(cpu, operand);
    if (!pt) pt=register_indirect_addressing(cpu, operand);
    return pt;
}


