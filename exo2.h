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

#endif




