#include "exo4.h"

CPU * cpu_init(int memory_size){
    CPU * cpu = (CPU*)malloc(sizeof(CPU));
    cpu->memory_handler = memory_init(memory_size);
    cpu->context = hashmap_create();
    hashmap_insert(cpu->context, "AX", int_to_point(0));
    hashmap_insert(cpu->context, "BX", int_to_point(0));
    hashmap_insert(cpu->context, "CX", int_to_point(0));
    hashmap_insert(cpu->context, "DX", int_to_point(0));
    return cpu;
}

void cpu_destroy(CPU *cpu){
    free_memory_handler(cpu->memory_handler);
    hashmap_destroy(cpu->context);
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
    for(int i = 0; i<data_count;i++){
        char * operand2 = data_instructions[i]->operand2;
        int k = 0;
        char buffer[50];
        int is_array = 0;
        while (operand2[k]!='\0')
        {
            if (operand2[k]==','){
                is_array = 1;
                int value;
                sscanf(buffer,"%d",&value);
                store(cpu->memory_handler,"DS");
                strcpy(buffer,"");
            } else {
                buffer[k] = operand2[k];
            }
            k++;
        }
        int value;
        sscanf(buffer,"%d",&value);
        store(cpu->memory_handler,"DS");
        

    }
}

