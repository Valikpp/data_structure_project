#include "exo6.h"

int main(){
    CPU * cpu = setup_test_environment();
    ParserResult * parser = parse("setup_env_test.txt");
    parser_show(parser);

    Instruction ** inst=parser->code_instructions; //tableau d'instructions de CODE de notre pseudo assembleur
    
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

    print_data_segment(cpu);
    print_hashmap_int(cpu->context);
    
    free_parser_result(parser);
    cpu_destroy(cpu);
    return 0;
}