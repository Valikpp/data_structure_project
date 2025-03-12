#include "exo3.h"

Instruction *parse_data_instruction(const char * line, HashMap *memory_locations){
    static int nb_occ = 0;
    char mnemonic[256];
    char operand1[256];
    char operand2[256];
    sscanf(line,"%s %s %s", mnemonic, operand1, operand2);
    int i = 0;
    int count = 1;
    while(operand2[i]!='\0'){
        if (operand2[i]==','){
            count++;
        }
        i++;
    }
    Instruction * inst = (Instruction*)malloc(sizeof(Instruction));
    inst->mnemonic = strdup(mnemonic);
    inst->operand1 = strdup(operand1);
    inst->operand2 = strdup(operand2);
    int * value = malloc(sizeof(int));
    *value = nb_occ;
    hashmap_insert(memory_locations,mnemonic,value);
    printf("%d\n",nb_occ);
    nb_occ += count;
    return inst;
}

Instruction *parse_code_instruction(const char *line,HashMap *labels, int code_count){
    int i=0;
    int countesp=1;
    int countoper=1;
    while (line[i]!='\0'){
        if (line[i]==' '){
            countesp++;
        }
        if (line[i]==','){
            countoper++;
        }

        i++;
    }
    char mnemonic[256];
    char operand1[256];
    char operand2[256];

    if (countesp>2){
        char label[256];
        sscanf(line,"%s %s %s,%s",label, mnemonic, operand1, operand2);
        int *code_c = malloc(sizeof(int));
        *code_c = code_count;
        hashmap_insert(labels,strdup(label),code_c);
    }else{
        sscanf(line,"%s %s,%s", mnemonic, operand1, operand2);
    }





}

