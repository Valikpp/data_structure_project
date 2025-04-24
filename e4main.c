#include "exo4.h"

int main(){
    CPU * cpu = cpu_init(512);
    ParserResult * parser = parse("test_pseudo_asm.txt");
    parser_show(parser);
    allocate_variables(cpu,parser->data_instructions,parser->data_count);
    print_data_segment(cpu);
    free_parser_result(parser);
    cpu_destroy(cpu);
    return 0;
}