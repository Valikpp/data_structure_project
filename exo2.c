#include "exo2.h"

MemoryHandler *memory_init(int size){
    /*
        Memory Handler initialization:
        Initializes structure MemoryHandler by assigning initial default values and initializing substructures
    
        Input: 
            int size - size of memory
        Output: 
            MemoryHandler * new_handler
    */
    MemoryHandler *handler = (MemoryHandler*)malloc(sizeof(MemoryHandler));
    assert(handler);
    handler->total_size = size;
    Segment * segment = (Segment*)malloc(sizeof(Segment));
    segment->next = NULL;
    segment->start = 0;
    segment->size = size;
    handler->free_list = segment;
    handler->allocated = hashmap_create();
    handler->memory = (void**)calloc(size,sizeof(void*));
    return handler;
}

Segment* find_free_segment(MemoryHandler* handler, int start, int size, Segment ** prev){
    
    /*
        Searching for an empty memory segment: 
        The function checks if a memory segment can be allocated at the specified coordinates and, if found successfully, returns the address of the segment containing the desired segment.
        
        Input: 
            MemoryHandler * handler
            int start - desired start position 
            int size - desired segment's length
            Segment ** prev - pointer to previous segment
        Output: 
            Segment * result - pointer to the segment containing the desired segment
    */
    
    if (!handler) return NULL;
    // search from first unallocated segment
    Segment * free_list = handler->free_list;
    Segment * prec = NULL;
    while (free_list){
        // Current segment satisfies desired start and size
        if((free_list->start <= start)&&(free_list->size>=size)){
            *prev=prec;
            return free_list; 
        }
        prec = free_list;
        free_list = free_list->next;
    }
    // Memory segment can not be allocated at the specified coordinates 
    return NULL;
}

int create_segment(MemoryHandler * handler,const char *name,int start, int size){
    
    /*
        Segment Allocation:
        The function allocates the desired segment in memory by removing it from the list of free segments and updates the list of available segments
        
        Input: 
            MemoryHandler * handler
            int start - desired start position 
            int size - desired segment's length
            string name - name of segment (key in "allocated" Hashmap) 
        Output: 
            0 - unsuccessful creation
            1 - successful allocation
    */
    
    Segment * prev;
    Segment * seg_libre = find_free_segment(handler,start,size,&prev);
    if(!seg_libre){
        return 0;
    }
    //Creation of subsegments
    Segment * before = NULL;
    Segment * new_seg;
    Segment * after = NULL;
    //Free memory space before desired segment
    if(seg_libre->start<start){
        before = malloc(sizeof(Segment));
        assert(before);
        before->start = seg_libre->start;
        before->size = start-seg_libre->start; //initialize next by NULL
        //merging with previous
        if(prev){
            prev->next = before;
        } else{
            handler->free_list = before;
        }
        prev = before;
    }
    //Free memory space after desired segment
    if(seg_libre->start+seg_libre->size > start+size){
        after = malloc(sizeof(Segment));
        assert(after);
        after->start = start+size;
        after->size = seg_libre->size - start - size;
        //liason avec le suivant
        after->next = seg_libre->next; // ?? prq on a echangé? before->next=after  
        seg_libre->next = after;           //after->next=seg_libre->after. 
    }
    // Desired segment allocation
    new_seg = malloc(sizeof(Segment));
    assert(new_seg);
    new_seg->start = start;
    new_seg->size = size;
    // Pair <seg_name , Segment*> inserted in handler->allocated
    hashmap_insert(handler->allocated,name,new_seg);
    if(prev){
        prev->next = seg_libre->next; 
    } else{
        handler->free_list = seg_libre->next; 
    }

    // Allocation of required memory
    handler->memory[start] = malloc(size);  
    assert(handler->memory[start]);

    free(seg_libre);
    return 1;
}

int remove_segment(MemoryHandler * handler, const char *name){
    
    /*
        Delete Selected Segment:
        Deletes the segment with the specified name from the table of selected segments and returns it to the free segment partition.
        When a segment is returned to free, it is merged with neighboring free segments (if possible) to avoid memory fragmentation.    

        Input: 
            MemoryHandler * handler
            string name - name of segment (key in "allocated" Hashmap) 
        Output: 
            0 - unsuccessful deletion
            1 - successful deletion
    */
    
    Segment *aliberer= (Segment*)hashmap_get(handler->allocated,name);
    //element does not exist / is not allocated
    if (aliberer==NULL) return 0;
    //remove segment from hashmap
    hashmap_remove(handler->allocated,name);
    Segment *courant=handler->free_list;
    int merged=0;
    Segment *before;
    Segment *prec=NULL;
    //merge with previous
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
   
    //merge with next 
     while (courant){
        if (aliberer->start+aliberer->size==courant->start){
            if (!merged){
                courant->start=aliberer->start;
                courant->size+=aliberer->size;
                merged=1;
            }else{
            before->size+=courant->size;
            Segment *tmp=courant->next;

            //current suppression
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
        //addition in head (we do ordered?)
        aliberer->next=handler->free_list;
        handler->free_list=aliberer;


    }
    free(aliberer);

    return 1;
}

void free_memory_handler(MemoryHandler * handler){
    /*
        Delete MemoryHandler:
        The function clears the memory space occupied by the structure  
        Input: 
            MemoryHandler * handler
        Output: NULL
    */
    hashmap_destroy(handler->allocated);
    for(int i = 0; i<handler->total_size;i++){
        if(handler->memory[i]){
             free(handler->memory[i]);
        }
    }
    free(handler->memory);
    Segment * courant = handler->free_list;
    Segment * tmp = NULL;
    while(courant){
        tmp = courant;
        courant= courant->next;
        free(tmp);
    }
    free(handler);
}

//Function out of task to check the work of create_segment
void afficher_liste_libre(MemoryHandler *handler){
    Segment * tmp = handler->free_list;
    printf("Liste de segments : ");
    while(tmp){
        printf("[%d, %d],",tmp->start, tmp->size);
        tmp = tmp->next;
    }
    printf("\n");
}