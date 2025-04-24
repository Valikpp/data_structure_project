#include "exo3.h"

int main(){
    MemoryHandler * handler = memory_init(1024);
    print_free_list(handler);
    create_segment(handler,"data",200,100);
    print_free_list(handler);
    remove_segment(handler,"data");
    print_free_list(handler);
    free_memory_handler(handler);
    return 0; 
}