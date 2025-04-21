#include "exo4.h"

CPU * cpu_init(int memory_size){ //?
    if (memory_size<128) return NULL;
    CPU * cpu = (CPU*)malloc(sizeof(CPU));
    cpu->memory_handler = memory_init(memory_size);
    create_segment(cpu->memory_handler,"SS",0,128);

    cpu->context = hashmap_create();
    hashmap_insert(cpu->context, "AX", int_to_point(0));
    hashmap_insert(cpu->context, "BX", int_to_point(0));
    hashmap_insert(cpu->context, "CX", int_to_point(0));
    hashmap_insert(cpu->context, "DX", int_to_point(0));

    hashmap_insert(cpu->context, "IP", int_to_point(0));
    hashmap_insert(cpu->context, "ZF", int_to_point(0));
    hashmap_insert(cpu->context, "SF", int_to_point(0));

    hashmap_insert(cpu->context, "SP", int_to_point(127));
    hashmap_insert(cpu->context, "BP", int_to_point(127));

    hashmap_insert(cpu->context, "ES", int_to_point(-1));


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
    return (handler->memory[seg->start+pos]);
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
        print_instruction_exp(data_instructions[i]);
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
    printf("==== Content of data segment <DS> ====\nDS = [");
    for(int i=0;i<segment->size;i++){
        if (cpu->memory_handler->memory[segment->start+i]){
            printf("%d,",*(int*)cpu->memory_handler->memory[segment->start+i]);
        } else {
            printf("_,");
        }
    }
    printf("]\n");
    printf("=== END of data segment <DS> ===\n");
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
        print_instruction_exp(data_instructions[i]);
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
    if (value>=cpu->memory_handler->total_size || value<0){
        return NULL;
    }
    return cpu->memory_handler->memory[value]; //load?
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
    if (!src || !dest) return;
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
        create_segment(cpu->memory_handler, "DS", cpu->memory_handler->free_list->start, 20);

        // Initialiser le segment de données avec des valeurs de test
        for (int i = 0; i < 10; i++) {
            int *value = (int *)malloc(sizeof(int));
            *value = i * 10 + 5;  // Valeurs 5, 15, 25, 35...
            printf("value: %d ",i);
            void *succ= store(cpu->memory_handler, "DS", i, value);
            printf("store succ: %d ",*(int*)succ);
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
    if (!pt) pt=segment_override_addressing(cpu,operand);
    return pt;
}

int push_value(CPU *cpu, int value){ //retourne -1 et pas 0 en cas d'erreur
    int *sp=hashmap_get(cpu->context,"SP");
    if (*sp<0) return -1;
    void* s=store(cpu->memory_handler,"SS",*sp,int_to_point(value));
    if (!s) return -1;
    (*sp)--;
    return 1;
}

int pop_value(CPU *cpu, int* dest){ //caster le void* à l'appel?
    int *sp=hashmap_get(cpu->context,"SP");
    if (*sp>=128) return -1;
    void*l=load(cpu->memory_handler,"SS",(*sp)+1);
    if (!l) return -1;
    *dest=*((int *)l);
    (*sp)++;
    return 1;
}

void *segment_override_addressing(CPU *cpu, const char *operand){
    char * pattern = "^\\[[(D|C|S|E)S:[A-D]X\\]$";
    if (!matches(pattern,operand)){
        return NULL;
    }
    char seg[3];
    char regist[3];
    sscanf(operand,"[%s:%s]",seg,regist);
    int *vregist=(int *)hashmap_get(cpu->context,regist);
    if (!vregist) return NULL;

    return load(cpu->memory_handler,seg,*vregist);

}

int find_free_address_strategy(MemoryHandler *handler, int size, int strategy){
    Segment *list=handler->free_list;
    int min,max,res;
    switch (strategy) {
        case 0:
            while (list){
                if (list->size>=size) return list->start;
                list=list->next;
            }
            break;
        case 1:
            min = INT_MAX;
            res=-1;
            while (list){
                int dif=list->size-size;
                if (dif>=0 && dif<min) {
                min=dif;
                res=list->start;
                }
                list=list->next;
            }
            return res;
            break;
        case 2:
            max=-1;
            res=-1;
            while (list){
                if (list->size>max && list->size>=max) res=list->start;
                list=list->next;
            }
            return res;
            break;
        

    }
    return -1;
}  

int alloc_es_segment(CPU* cpu){
    int* ax=hashmap_get(cpu->context,"AX");
    int *bx=hashmap_get(cpu->context,"BX");
    int start=find_free_address_strategy(cpu->memory_handler,*ax,*bx);
    int succ=create_segment(cpu->memory_handler,"ES",start,*ax);

    int *zf=hashmap_get(cpu->context,"ZF");
    *zf=!(succ);

    for (int i=0;i<*ax;i++){
        store(cpu->memory_handler,"ES",i,int_to_point(0)); //constant pool? add it directly in int to point maybe
    }

    int* es=hashmap_get(cpu->context,"ES");
    *es=start;


}

int free_es_segment(CPU* cpu){
    Segment* esseg=hashmap_get(cpu->memory_handler->allocated,"ES");
    if  (esseg==NULL) return 0;

    for (int i=0;i<esseg->size;i++){
        free(load(cpu->memory_handler,"ES",i)); //constant pool? add it directly in int to point maybe
    }

    int succ=hashmap_remove(cpu->memory_handler->allocated,"ES");
    if (succ==0) return succ;

    int* es=hashmap_get(cpu->context,"ES");
    *es=-1;

    return 1;

}














