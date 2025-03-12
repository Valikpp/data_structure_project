#ifndef EXO2
#define EXO2
#include "exo1.h"

typedef struct segment{
    int start;
    int size;
    struct segment * next;
} Segment;

typedef struct memoryHandler{
    void **memory;
    int total_size;
    Segment * free_list;
    HashMap * allocated;
} MemoryHandler;

MemoryHandler *memory_init(int size);

Segment* find_free_segment(MemoryHandler* handler, int start, int size, Segment ** prev);

int create_segment(MemoryHandler * handler,const char *name,int start, int size);

void afficher_liste_libre(MemoryHandler *handler);

int remove_segment(MemoryHandler * handler, const char *name);

#endif




