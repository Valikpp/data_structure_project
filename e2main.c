#include "exo2.h"

int main(){
    MemoryHandler * handler = memory_init(1024);
    afficher_liste_libre(handler);
    create_segment(handler,"data",200,100);
    afficher_liste_libre(handler);
    hashmap_destroy(handler->allocated);
    return 0;
}