#ifndef EXO4
#define EXO4
#include "exo3.h"
#include <regex.h>

typedef struct {
    MemoryHandler * memory_handler; //Gestionnaire de memoire
    HashMap *context; // Registres (AX,DX,CX,DX)
    HashMap *constant_pool; // Table de hachage pour stocker les valeurs immdiates
} CPU;

CPU * cpu_init(int memory_size);

void cpu_destroy(CPU *cpu);

void allocate_variables(CPU *cpu, Instruction** data_instructions,int data_count);
void print_data_segment(CPU * cpu);

int matches(const char *pattern, const char *string);
void * immediate_addressing(CPU * cpu, const char * operand);
void *register_addressing(CPU * cpu, const char*operand);
void *memory_direct_addressing(CPU * cpu, const char*operand);
void * register_indirect_addressing(CPU * cpu, const char*operand);
void handle_MOV(CPU * cpu, void * src, void *dest);
CPU *setup_test_environment();
void *resolve_addressing(CPU *cpu, const char *operand);

int push_value(CPU *cpu, int value);
int pop_value(CPU *cpu, int* dest);

void *segment_override_addressing(CPU *cpu, const char *operand);
int find_free_address_strategy(MemoryHandler *handler, int size, int strategy);
int alloc_es_segment(CPU* cpu);
int free_es_segment(CPU* cpu);

#endif