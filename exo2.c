#include "exo2.h"

MemoryHandler *memory_init(int size){
    MemoryHandler *handler = (MemoryHandler*)malloc(sizeof(MemoryHandler));
    assert(handler);
    handler->total_size = size;
    Segment * segment = (Segment*)malloc(sizeof(Segment));
    segment->next = NULL;
    segment->start = 0;
    segment->size = size;
    handler->free_list = segment;
    handler->allocated = hashmap_create();
    handler->memory = (void**)malloc(sizeof(void*)*size);
    return handler;
}

Segment* find_free_segment(MemoryHandler* handler, int start, int size, Segment ** prev){
    Segment * free_list = handler->free_list;
    Segment * tmp = handler->free_list;
    while (free_list)
    {
        if((free_list->start <= start)&&(free_list->size>=size)){
            if(tmp==handler->free_list){
                *prev = NULL;
            }else{
                *prev = tmp; 
            }
            return free_list; 
        }
        tmp = free_list;
        free_list = free_list->next;
    }
    return NULL;
}

int create_segment(MemoryHandler * handler,const char *name,int start, int size){
    Segment * prev;
    Segment * seg_libre = find_free_segment(handler,start,size,&prev);
    if(!seg_libre){
        return 0;
    }
    //Creation des sous-segments
    Segment * before = NULL;
    Segment * new_seg;
    Segment * after = NULL;
    if(seg_libre->start<start){
        before = malloc(sizeof(Segment));
        assert(before);
        before->start = seg_libre->start;
        before->size = start-seg_libre->start;
        //liason avec le precedent
        if(prev){
            prev->next = before;
        } else{
            handler->free_list = before;
        }
        prev = before;
    }
    if(seg_libre->start+seg_libre->size > start+size){
        after = malloc(sizeof(Segment));
        assert(after);
        after->start = start+size;
        after->size = seg_libre->size - start - size;
        //liason avec le suivant
        after->next = seg_libre->next;
        seg_libre->next = after;
    }
    new_seg = malloc(sizeof(Segment));
    assert(new_seg);
    new_seg->start = start;
    new_seg->size = size;
    hashmap_insert(handler->allocated,name,new_seg);
    prev->next = seg_libre->next;

    handler->memory[start] = malloc(size);  
    assert(handler->memory[start]);

    free(seg_libre);
    return 1;
}

int remove_segment(MemoryHandler * handler, const char *name){
    return 1;
}


//Foncton hors enonce pour verifier le travail de create_segment
void afficher_liste_libre(MemoryHandler *handler){
    Segment * tmp = handler->free_list;
    printf("Liste de segments : ");
    while(tmp){
        printf("[%d, %d],",tmp->start, tmp->size);
        tmp = tmp->next;
    }
    printf("\n");
}