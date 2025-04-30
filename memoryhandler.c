#include "memoryhandler.h"

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
    assert(segment);
    segment->next = NULL;
    segment->start = 0;
    segment->size = size;
    handler->free_list = segment;
    handler->allocated = hashmap_create();
    if (handler->allocated==NULL){
         printf("Error memory_init : hashmap creation failed \n");
         return NULL;
    }
    handler->memory = (void**)calloc(size,sizeof(void*));
    assert(handler->memory);
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
            Segment ** prev - pointer to previous segment ( to return previous segment of the segment found)
        Output: 
            Segment * result - pointer to the segment containing the desired segment
    */
    
    if (handler==NULL){
         printf("Error find_free_segment : no handler to look for segment \n");
         return NULL;
    }
    // search from first unallocated segment
    Segment * free_list = handler->free_list;
    Segment * prec = NULL;
    while (free_list){
        // Current segment satisfies desired start and size
        if((free_list->start <= start && start<=free_list->start+ free_list->size)){
            if(free_list->start + free_list->size >= start + size){
            *prev=prec;
            return free_list;
            }else{
                printf("Error find_free_segment : free segment containing start position not large enough \n");
                return NULL;
            } 
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
    
    if (handler==NULL){
         printf("Error find_free_segment : no handler to create segment in \n");
         return 0;
    }
    if (name==NULL){
         printf("Error find_free_segment : no name for segment to be allocated \n");
         return 0;
    }

    Segment * prev;
    Segment * seg_libre = find_free_segment(handler,start,size,&prev);
    if(!seg_libre){
        printf("Error create_segment : no free segment large enough was found to allocate new segment \n");
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
        //link with the previous
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
        //link with the following
        after->next = seg_libre->next;  
        seg_libre->next = after;          
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

    free(seg_libre);
    seg_libre = NULL;
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
    
    if (handler==NULL){
         printf("Error remove_segment : no handler to remove segment from \n");
         return 0;
    }

    if (name==NULL){
         printf("Error remove_segment : no name for the segment to be removed \n");
         return 0;
    }
    Segment *aliberer= (Segment*)hashmap_get(handler->allocated,name);
    //element does not exist / is not allocated
    if (aliberer==NULL) return 0;
    //remove segment from hashmap
    
    if (hashmap_remove(handler->allocated,name)==0){
         printf("Error remove_segment : segment could not be removed from hashmap \"allocated\" \n");
         return 0;
    }
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
        aliberer->next=handler->free_list;
        handler->free_list=aliberer;


    }
    free(aliberer);
    aliberer = NULL;
    return 1;
}

int find_free_address_strategy(MemoryHandler *handler, int size, int strategy){
    /*
        Segment choice with selected strategy
        The function seeking for a free memory segment according to the strategy: 

            - First Fit (0): Allocates the first sufficiently large free memory segment.
            
            - Best Fit (1): Finds the memory segment whose size is closest to the requested size,
              with a view to minimising potential wastage.
            
            - Worst Fit (2): Selects the segment with the largest remaining free space.

        Input:
            MemoryHandler *handler -- initialized Memory Handler
            int size -- minimal size required for segment
            int strategy -- 
        Output: 
    */
    Segment *list=handler->free_list;
    int min,max,res;
    switch (strategy) {
        case 0:
            // First free segment
            while (list){
                if (list->size>=size) return list->start;
                list=list->next;
            }
            break;
        case 1:
            min = INT_MAX;
            res=-1;
            // Segment with minimal potential wastage
            while (list){
                int dif=list->size-size;
                if (dif>=0 && dif<min) {
                min=dif;
                res=list->start;
                }
                list=list->next;
            }
            return res;
            break;
        case 2:
            max=-1;
            res=-1;
            // Segment with largest remaining free space
            while (list){
                if (list->size>max && list->size>=max) res=list->start;
                list=list->next;
            }
            return res;
            break;
        

    }
    return -1;
}  


//Function out of task to check the work of create_segment
void print_free_list(MemoryHandler *handler){
    /*
        Utility function showing a list of free segments
    */
    
    if (handler==NULL){
        printf("Error print_free_list : no handler to print free_list \n");
        return ;
   }
    
    Segment * tmp = handler->free_list;
    printf("Free Segments List: ");
    while(tmp){
        printf("[%d, %d],",tmp->start, tmp->size);
        tmp = tmp->next;
    }
    printf("\n");
}


void* store(MemoryHandler *handler, const char *segment_name, int pos, void *data){
    /*
        Save to memory function
        The function storing a data at desired position of segment
        Input: 
            MemoryHandler * handler -- initialized memory handler of CPU
            char * segment_name -- name of desired segment
            int pos -- position in segment (!!! RELATIONAL PARAMETER !!!)
            void * data -- data to save at segment[pos]
        Output: 
            data in case of success
            NULL in case of error
    */
    Segment *seg=(Segment *)hashmap_get(handler->allocated,segment_name);
    if (seg==NULL){
        printf("Error store : segment not found in \"allocated\" hashmap\n");
        return NULL;
    }
    if (pos >= seg->size){
        printf("Segmentation fault store in memory table: value out of bounds\n");
        return NULL;
    }
    if (handler->memory[seg->start + pos]!=NULL){
        free(handler->memory[seg->start + pos]);
    }
    handler->memory[seg->start + pos] = data;
    return data;
}

void *load(MemoryHandler *handler, const char *segment_name, int pos){
    /*
        Load from memory function
        The function loading a data from desired position of segment
        Input: 
            MemoryHandler * handler -- initialized memory handler of CPU
            char * segment_name -- name of desired segment
            int pos -- position in segment (!!! RELATIONAL PARAMETER !!!)
        Output: 
            data in case of success
            NULL in case of error
    */
    Segment *seg=(Segment *)hashmap_get(handler->allocated,segment_name);
    if (seg==NULL){
        printf("Error load : segment not found in \"allocated\" hashmap\n");
        return NULL;
    }
    if (pos >= seg->size){
        printf("Segmentation fault load in memory table: value out of bounds\n");
        return NULL;
    }
    return (handler->memory[seg->start+pos]);
}
