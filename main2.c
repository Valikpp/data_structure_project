#include "program.h"

int main(){
    /*
        This main tests MemoryHandler management functions 
    */
    printf("===== This main tests MemoryHandler management functions =====\n");
    
    //Memory Handler creation  
    MemoryHandler * handler = memory_init(1024);

    // Show free list
    print_free_list(handler);

    // Create segment
    printf("\nAllocation of segment named \"Data\" in position 200 with size 100\n");
    create_segment(handler,"data",200,100);
    print_free_list(handler);

    // Remove created segment
    printf("\nDeletion of segment named \"Data\", automatic union of segments to avoid memory fragmentation\n");
    remove_segment(handler,"data");
    print_free_list(handler);

    //free MemoryHandler
    free_memory_handler(handler);
    printf("===== End of main2 =====\n");
    return 0; 
}