#include "program.h"

int main(){
    /*
        This main tests program running management functions 
    */
    printf("===== This main tests program running management functions =====\n");

    CPU * cpu = cpu_init(1024);
    ParserResult * parser = parse("test2.txt");

    printf("\n===========Parser after file scanning==============\n");
    parser_show(parser);

    printf("\n===========Data allocation in memory==============\n");
    allocate_variables(cpu,parser->data_instructions,parser->data_count);
    //print_data_segment(cpu);

    printf("\n===========Constant resolving==============\n");
    resolve_constants(parser);

    printf("\n===========Code allocation in memory==============\n");
    allocate_code_segment(cpu,parser->code_instructions,parser->code_count);

    printf("\n===========Parser final state=============\n");
    parser_show(parser);

    run_program(cpu);

    free_parser_result(parser);
    cpu_destroy(cpu);
    printf("===== End of main6 =====\n"); 
    return 0;

}