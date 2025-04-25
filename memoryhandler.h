#ifndef MEMORYHANDLER
#define MEMORYHANDLER
#include "hashmap.h"

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

int find_free_address_strategy(MemoryHandler *handler, int size, int strategy);

void print_free_list(MemoryHandler *handler);

int remove_segment(MemoryHandler * handler, const char *name);


void* store(MemoryHandler *handler, const char *segment_name, int pos, void *data);
void *load(MemoryHandler *handler, const char *segment_name, int pos);



#endif




