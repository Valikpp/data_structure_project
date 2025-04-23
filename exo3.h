#ifndef EXO3
#define EXO3
#define NBMININST 30
#include "exo2.h"

extern int nb_data_occ;

typedef struct _instruction{
    char * mnemonic; //Instruction mnemonic (ou nom de variable pour .DATA)
    char * operand1; //Premier operande (ou type pour .DATA)
    char * operand2; //Seconde operande (ou initisalisation pour .DATA)
} Instruction; 

typedef struct _parserresult{
    Instruction **data_instructions;  // Tableau d'instructions .DATA
    int data_count;                   //Nombre d'instructions .DATA
    Instruction **code_instructions;  // Tableau d'instructions .CODE
    int code_count;                   //Nombre d'instructions .CODE
    HashMap *labels;                  //labels -> indices dans code_instrcutions
    HashMap *memory_locations;        //noms de variables -> adresse memoire
} ParserResult;

Instruction *parse_data_instruction(const char * line, HashMap *memory_locations);

Instruction *parse_code_instruction(const char *line,HashMap *labels, int code_count);

ParserResult *parse(const char *filename);

void print_instruction_exp(Instruction* inst);

void print_instruction(Instruction *inst);

void parser_show(ParserResult * parser);

void free_parser_result(ParserResult *result);

void free_memory_handler(MemoryHandler * handler);

void free_instruction(Instruction *inst);

void* store(MemoryHandler *handler, const char *segment_name, int pos, void *data);
void *load(MemoryHandler *handler, const char *segment_name, int pos);

#endif