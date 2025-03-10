#include "exo1.h"

int main(){
    HashMap * map = hashmap_create();
    char * str = "test_string";
    hashmap_insert(map,"t1",str);
    char * str2 = "test_string2";
    hashmap_insert(map,"t2",str2);
    printf("%s\n", (char *)hashmap_get(map,"t1"));
    hashmap_remove(map,"t1");
    hashmap_destroy(map);
    return 0;
}