#include "exo3.h"

int main(){
    HashMap * memory = hashmap_create();
    Instruction * inst2 = parse_data_instruction("arr DW 5,6,7,8,890", memory);
    Instruction * inst = parse_data_instruction("X DW 3", memory);
    printf("%s | %s | %s\n", inst->mnemonic, inst->operand1, inst->operand2);
    return 0;
}