#include "exo3.h"

int main(){
    /*
        This main tests Parser management functions 
    */
    printf("===== This main tests Parser management functions =====\n");
    //parsing of pseudo-assembler
    ParserResult * parser = parse("test_pseudo_asm.txt");
    
    parser_show(parser);

    //Manual free of instructions because there is no CPU responsible for this operation
    for (int i = 0; i<parser->data_count;i++){
        free_instruction(parser->data_instructions[i]);
    }
    for (int i = 0; i<parser->code_count;i++){
        free_instruction(parser->code_instructions[i]);
    }
    free_parser_result(parser);
    return 0;
    printf("===== End of main3 =====\n");
}