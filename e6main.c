#include "exo6.h"

int main(){
CPU * cpu = setup_test_environment();
    ParserResult * parser = parse("setup_env_test.txt");
    parser_show(parser);

    Instruction ** inst=parser->code_instructions; //tableau d'instructions de CODE de notre pseudo assembleur
    
    print_data_segment(cpu);
    print_hashmap_int(cpu->context);




    free_parser_result(parser);
    cpu_destroy(cpu);
    return 0;

}