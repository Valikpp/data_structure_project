#include "program.h"

int main(int argc, char *argv[]){

    CPU * cpu = cpu_init(1024);
    if(!argv[1]){
        printf("Please enter file name in format ./compile \"programm.txt\"\n");
        return 0;
    }
    ParserResult * parser = parse(argv[1]);
    allocate_variables(cpu,parser->data_instructions,parser->data_count);
    resolve_constants(parser);
    allocate_code_segment(cpu,parser->code_instructions,parser->code_count);

    Segment *ds=hashmap_get(cpu->memory_handler->allocated,"DS");
    if (!ds) return 0;

    Instruction *courant=fetch_next_instruction(cpu);
    while (courant){
       
        int e=execute_instruction(cpu,courant);
        if (!e) return 0;
        courant=fetch_next_instruction(cpu);
    }
    printf("CPU FINAL STATE : \n");
    print_data_segment(cpu);
    print_hashmap_int(cpu->context);
    printf("\n");
    free_parser_result(parser);
    cpu_destroy(cpu);
    return 0;
}