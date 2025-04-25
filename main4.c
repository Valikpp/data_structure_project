#include "exo6.h"

int main(){
    /*
        This main tests CPU management functions 
    */
    printf("===== This main tests CPU management functions =====\n");

    //CPU initialization
    CPU * cpu = cpu_init(512);

    //Parsing of pseudo-assembly programme
    ParserResult * parser = parse("test_pseudo_asm.txt");
    parser_show(parser);
    //variables allocation
    allocate_variables(cpu,parser->data_instructions,parser->data_count);
    printf("\n--- Allocation of variables ---\n");
    print_data_segment(cpu);

    // Code segment allocation
    allocate_code_segment(cpu,parser->code_instructions,parser->code_count);

    free_parser_result(parser);
    cpu_destroy(cpu);
    printf("===== End of main4 =====\n");

    return 0;
}