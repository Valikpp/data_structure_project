#include "exo2.h"

int main(){
    MemoryHandler * handler = memory_init(1024);
    afficher_liste_libre(handler);
    create_segment(handler,"data",200,100);
    afficher_liste_libre(handler);
    remove_segment(handler,"data");
    afficher_liste_libre(handler);
    free_memory_handler(handler);
    return 0;
}