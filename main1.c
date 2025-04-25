#include "program.h"

int main(){

    /*
        This main tests hashmap management functions 
    */
    printf("===== This main tests hashmap management functions =====\n");
    HashMap * map = hashmap_create();

    // dynamically allocated values
    int * five = malloc(sizeof(int));
    *five = 5;
    int * nine = malloc(sizeof(int));
    *nine = 9;

    // insertion
    hashmap_insert(map,"five",five);
    hashmap_insert(map,"nine",nine);
    // access
    printf("Try to get value associated to key \"five\": value=%d\n", *(int *)hashmap_get(map,"five"));
    //print of content
    printf("--- Showing all content ---\n");
    print_hashmap_int(map);
    printf("--- End of content ---\n");

    //deletion from hashmap  
    hashmap_remove(map,"five");
    free(five);
    // destruction with memory deallocation
    hashmap_destroy(map);
    printf("===== End of main1 =====\n");
    return 0;
}