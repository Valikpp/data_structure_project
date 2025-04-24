#include "exo3.h"

int main(){
    ParserResult * parser = parse("test_pseudo_asm.txt");
    parser_show(parser);
    free_parser_result(parser); //leak because instructions must be deleted in CPU
}