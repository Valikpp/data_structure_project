#include "exo1.h"

int main(){
    HashMap * map = hashmap_create();


    int * five = malloc(sizeof(int));
    *five = 5;
    int * nine = malloc(sizeof(int));
    *nine = 9;
    //char * str = strdup("test_string");
    hashmap_insert(map,"five",five);
    //char * str2 = strdup("test_string2");
    hashmap_insert(map,"nine",nine);
    // printf("%s\n", (char *)hashmap_get(map,"t1"));
    // printf("%s\n", (char *)hashmap_get(map,"t2"));
    hashmap_show_pairs_Integer(map);
    hashmap_remove(map,"five");
    hashmap_destroy(map);
    return 0;
}