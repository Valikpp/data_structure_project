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
    handler->memory = NULL; //(void**)malloc(sizeof(void*)*size);
    return handler;
}

Segment* find_free_segment(MemoryHandler* handler, int start, int size, Segment ** prev){
    if (!handler) return NULL;
    Segment * free_list = handler->free_list;
    Segment * prec = NULL;
    while (free_list){
        if((free_list->start <= start)&&(free_list->size>=size)){
            *prev=prec;
            return free_list; 
        }
        prec = free_list;
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
        before->size = start-seg_libre->start; //initialiser next a NULL
        //liason avec le precedent
        if(prev){
            prev->next = before;
        } else{
            handler->free_list = before;
        }
        prev = before;
    }
    //il existe de l'espace libre après le nouveau segment
    if(seg_libre->start+seg_libre->size > start+size){
        after = malloc(sizeof(Segment));
        assert(after);
        after->start = start+size;
        after->size = seg_libre->size - start - size;
        //liason avec le suivant
        after->next = seg_libre->next; // ?? prq on a echangé? before->next=after  
        seg_libre->next = after;           //after->next=seg_libre->after. 
    }
    new_seg = malloc(sizeof(Segment));
    assert(new_seg);
    new_seg->start = start;
    new_seg->size = size;
    hashmap_insert(handler->allocated,name,new_seg);
    if(prev){
        prev->next = seg_libre->next; 
    } else{
        handler->free_list = seg_libre->next; 
    }

    handler->memory[start] = malloc(size);  
    assert(handler->memory[start]);

    free(seg_libre);
    return 1;
}

int remove_segment(MemoryHandler * handler, const char *name){
    Segment *aliberer= (Segment*)hashmap_get(handler->allocated,name);
    //l'element n'existe pas/ n'est pas alloue
    if (aliberer==NULL) return 0;
    //on supprime de la hashmap
    hashmap_remove(handler->allocated,name);
    Segment *courant=handler->free_list;
    int merged=0;
    Segment *before;
    Segment *prec=NULL;
    //merge avec before
    while (courant){
        if (courant->start+courant->size==aliberer->start){
            courant->size+=aliberer->size;
            merged=1;
            before=courant;
            break;
        }
        courant=courant->next;  
    }
    courant=handler->free_list;
    //merge avec after
    
     while (courant){
        if (aliberer->start+aliberer->size==courant->start){
            if (!merged){
                courant->start=aliberer->start;
                courant->size+=aliberer->size;
                merged=1;
            }else{
            before->size+=courant->size;
            Segment *tmp=courant->next;

            //suppression du courant 
            if (prec==NULL) handler->free_list=courant->next;
            else prec->next=courant->next;
    
            free(courant);
            }

            break;
        }
        prec=courant;
        courant=courant->next;  
    }

    if (!merged){
        //ajout en tete (on fait ordonnés?)
        aliberer->next=handler->free_list;
        handler->free_list=aliberer;


    }
    free(aliberer);

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