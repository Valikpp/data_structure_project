#ifndef EXO3
#define EXO3
#include "exo2.h"

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
#endif