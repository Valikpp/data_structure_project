#include "exo6.h"

int main(){
    /*
        This main tests CPU management functions 
    */
    printf("===== This main tests addressing functions in special predefined context =====\n");

    // CPU initialization
    CPU * cpu = setup_test_environment();

    //Parsing of pseudo-assembly programme
    ParserResult * parser = parse("setup_env_test.txt");
    printf("\n--- PARSER initial state ---\n");
    parser_show(parser);

    Instruction ** inst=parser->code_instructions; //table of .CODE instructions
    
    printf("\n--- CPU initial state ---\n");
    print_data_segment(cpu);
    print_hashmap_int(cpu->context);
    
    for (int i=0;i<parser->code_count;i++){
        if (strcmp(inst[i]->mnemonic,"MOV")==0){
            void *pt1=register_addressing(cpu, inst[i]->operand1); //le premier operande est toujours un registre

            void *pt2=immediate_addressing(cpu,inst[i]->operand2);
            if (!pt2) pt2=register_addressing(cpu, inst[i]->operand2);
            if (!pt2) pt2=memory_direct_addressing(cpu, inst[i]->operand2);
            if (!pt2){
                pt2=register_indirect_addressing(cpu, inst[i]->operand2);
            }
            handle_MOV(cpu,pt2,pt1); //on met la valeur contenue dans le deuxieme operande, dans le premier

        }
    }
    printf("\n--- CPU final state ---\n");
    print_data_segment(cpu);
    print_hashmap_int(cpu->context);
    allocate_code_segment(cpu,parser->code_instructions,parser->code_count);

    for (int i = 0; i<parser->data_count;i++){
        free_instruction(parser->data_instructions[i]); //setup_test_env sets manually memory, so allocate_variables wasn't applied. As a result data instructions must be free manually
    }
    free_parser_result(parser);
    cpu_destroy(cpu);
    printf("===== End of main5 =====\n"); 
    return 0;
}