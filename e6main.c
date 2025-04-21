#include "exo6.h"

int main(){
    CPU * cpu = cpu_init(1024);
    ParserResult * parser = parse("setup_env_test.txt");

    parser_show(parser);
    allocate_variables(cpu,parser->data_instructions,parser->data_count);
    Instruction ** inst=parser->code_instructions; //tableau d'instructions de CODE de notre pseudo assembleur
    print_data_segment(cpu);

    printf("memory locations : \n \n");
    print_hashmap_int(parser->memory_locations);

    resolve_constants(parser);
    parser_show(parser);

    


    free_parser_result(parser);
    cpu_destroy(cpu);
    return 0;

}