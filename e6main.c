#include "exo6.h"

int main(){
    CPU * cpu = cpu_init(1024);
    ParserResult * parser = parse("setup_env_test.txt");

    //parser_show(parser);
    allocate_variables(cpu,parser->data_instructions,parser->data_count);
    print_data_segment(cpu);


    resolve_constants(parser);
    parser_show(parser);
    

    //allocate_code_segment(cpu,parser->code_instructions,parser->code_count);
    // printf("le code continue\n");
    // Segment *cs=hashmap_get(cpu->memory_handler->allocated,"CS");
    // for(int i=0;i<cs->size;i++){
    //     print_instruction_exp((Instruction *)load(cpu->memory_handler,"CS",i));
    // }

    

  
    // //parser_show(parser);
    // print_hashmap_int(cpu->context);
    // for (int i=0;i<parser->code_count;i++){
    // printf("i=%d \n" ,i);
    // execute_instruction(cpu,inst[i]);
    // print_hashmap_int(cpu->context);
    // }

    //run_program(cpu);


    

    free_parser_result(parser);
    cpu_destroy(cpu);
    return 0;

}