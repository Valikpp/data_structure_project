#include "exo4.h"

CPU * cpu_init(int memory_size){
    /*
        CPU initialization
        Initializes a object of type CPU with memory of desired size
        
        Input:
            int memory_size -- desired memory size (128 and more bytes)
        Output:
            CPU * cpu object
    */
    
    if (memory_size<128) {
        printf("Error cpu_init : CPU should be initialized at a bigger size\n");
        return NULL;
    }

    CPU *cpu = (CPU *)malloc(sizeof(CPU));
    cpu->memory_handler = memory_init(memory_size);
    if (cpu->memory_handler==NULL) { printf("Error cpu_init : failed to initialize memory handler \"SS\" \"context\"\n"); return NULL; }

    
    if (create_segment(cpu->memory_handler, "SS", 0, 128)==0) { printf("Error cpu_init : failed to create stack segment \"SS\"\n"); return NULL; }

    // registers initialization
    cpu->context = hashmap_create();
    if (cpu->context==NULL) { printf("Error cpu_init : failed to create hashmap \"context\"\n"); return NULL; }

    int succ = 0;
    succ = hashmap_insert(cpu->context, "AX", int_to_point(0));
    if (succ == 0) { printf("Error cpu_init : failed to insert in hashmap \"context\"\n"); return NULL; }

    succ = hashmap_insert(cpu->context, "BX", int_to_point(0));
    if (succ == 0) { printf("Error cpu_init : failed to insert in hashmap \"context\"\n"); return NULL; }

    succ = hashmap_insert(cpu->context, "CX", int_to_point(0));
    if (succ == 0) { printf("Error cpu_init : failed to insert in hashmap \"context\"\n"); return NULL; }

    succ = hashmap_insert(cpu->context, "DX", int_to_point(0));
    if (succ == 0) { printf("Error cpu_init : failed to insert in hashmap \"context\"\n"); return NULL; }

    succ = hashmap_insert(cpu->context, "IP", int_to_point(0));
    if (succ == 0) { printf("Error cpu_init : failed to insert in hashmap \"context\"\n"); return NULL; }

    succ = hashmap_insert(cpu->context, "ZF", int_to_point(0));
    if (succ == 0) { printf("Error cpu_init : failed to insert in hashmap \"context\"\n"); return NULL; }

    succ = hashmap_insert(cpu->context, "SF", int_to_point(0));
    if (succ == 0) { printf("Error cpu_init : failed to insert in hashmap \"context\"\n"); return NULL; }

    succ = hashmap_insert(cpu->context, "SP", int_to_point(127));
    if (succ == 0) { printf("Error cpu_init : failed to insert in hashmap \"context\"\n"); return NULL; }

    succ = hashmap_insert(cpu->context, "BP", int_to_point(127));
    if (succ == 0) { printf("Error cpu_init : failed to insert in hashmap \"context\"\n"); return NULL; }

    succ = hashmap_insert(cpu->context, "ES", int_to_point(-1));
    if (succ == 0) { printf("Error cpu_init : failed to insert in hashmap \"context\"\n"); return NULL; }

    cpu->constant_pool = hashmap_create();
    if (cpu->constant_pool==NULL) { printf("Error cpu_init : failed to create hashmap \"constant_pool\"\n"); return NULL; }


    return cpu;
}

void cpu_destroy(CPU *cpu){
    /*
        CPU destruction 
        The function clearing dynamic memory allocated by CPU
        
        Input:
            CPU * cpu -- CPU type object
    */
    if (cpu==NULL) { 
        printf("Error cpu_destroy : no cpu to destroy\n"); 
        return ; 
    }
    free_memory_handler(cpu->memory_handler);
    hashmap_destroy(cpu->context);
    hashmap_destroy(cpu->constant_pool);
    free(cpu);
    cpu=NULL;
}



void allocate_variables(CPU *cpu, Instruction** data_instructions,int data_count){
    /*
        Variables allocation
        The function creates a segment for Data and for every instruction of .DATA stores a value of variable in memory of CPU
        
        Input:
            CPU * cpu -- initialized CPU
            Instruction** data_instructions -- ParserResult->data_instructions object, list of .DATA instructions
            int data_count -- ParserResult->data_count
    */    
    // nb_data_occ -- external variable which contains the size of the space required to store the variables
    int succ=create_segment(cpu->memory_handler,"DS",cpu->memory_handler->free_list->start,nb_data_occ);
    if (succ==0) { 
        printf("Error allocate_variables : failed to create data segment \"DS\"\n"); 
        return ; 
    }
    //index in memory of CPU
    int pos=0;
    for(int i = 0; i<data_count;i++){
        char * operand2 = data_instructions[i]->operand2;
        //general index for char *
        int k = 0;
        char buffer[100];
        //index in buffer
        int buff_ind = 0;
        while (operand2[k]!='\0')
        {
            if (operand2[k]==','){
                buffer[buff_ind] = '\0';
                int value;
                sscanf(buffer," %d ",&value);
                store(cpu->memory_handler,"DS",pos,int_to_point(value));
                // buffer value is overwritten, reset to empty (by putting the index away)
                buffer[0] = '\0';
                buff_ind = 0;
                pos++;
            } else {
                buffer[buff_ind] = operand2[k];
                buff_ind++;
            }
            k++;
        }
        buffer[buff_ind] = '\0';
        int value;
        sscanf(buffer," %d ",&value);
        store(cpu->memory_handler,"DS",pos,int_to_point(value));
        pos++;
        free_instruction(data_instructions[i]);
        data_instructions[i]=NULL;
    }
}

void print_data_segment(CPU * cpu){
    /*
        Data segment's values printing
        The function prints all data of Data Segment stored in CPU memory
        
        Input:
            CPU * cpu -- initialized CPU

        "_" respresents an uninitialized case (Ex: Data Segment = [1,2,3,4,_,_,_])
    */  
    Segment * segment = (Segment*)hashmap_get(cpu->memory_handler->allocated,"DS");
    if(!segment) return;
    printf("==== Content of data segment <DS> ====\nDS = [");

    for(int i = 0; i < segment->size; i++){
        void *value = load(cpu->memory_handler, "DS", i);
        if (value) {
            printf("%d", *(int*)value);
        } else {
            printf("_");
        }

        if (i < segment->size - 1) {
            printf(",");
        }
    }

    printf("]\n");
    printf("=== END of data segment <DS> ===\n \n");
}

int matches(const char *pattern, const char *string) {
    /*
        Utility function matching a string with regex pattern
        Output:
            1 in case of match
            0 in other case
    */  
    regex_t regex;
    int result = regcomp(&regex, pattern, REG_EXTENDED);
    if (result) {
        fprintf(stderr, "Regex compilation failed for pattern: %s\n", pattern);
        return 0;
    }
    result = regexec(&regex, string, 0, NULL, 0);
    regfree(&regex);
    return result == 0;
}

void * immediate_addressing(CPU * cpu, const char * operand){
    /*
        Immediate addressing treatment
        Matches operand with immediate addressing pattern and updates pool of constants
    */ 
    char * pattern = "^[0-9]+$"; // string containing an integer
    if (!matches(pattern,operand)){
        return NULL;
    }
    int * result = (int *)hashmap_get(cpu->constant_pool,operand);
    if (result){
        return (void*)result;
    }
    int value;
    sscanf(operand,"%d",&value);
    result = int_to_point(value);
    hashmap_insert(cpu->constant_pool,operand,result);
    return (void*)result;  
}

void *register_addressing(CPU * cpu, const char*operand){
    /*
        Register addressing treatment
        Matches operand with register addressing pattern and returns a value of required register
    */ 
    char * pattern = "^[A-D]X$"; // Character between A and D + X
    if (!matches(pattern,operand)){
        return NULL;
    }
    int *result=(int *)hashmap_get(cpu->context,operand);
    if (!result) return NULL;

    return (void *) result;
}

void *memory_direct_addressing(CPU * cpu, const char*operand){
    /*
        Memory direct addressing treatment
        Matches operand with memory direct addressing pattern and returns a value saved in cpu's memory at required position
    */ 
    char * pattern = "^\\[[0-9]+\\]$"; //integer in []
    if (!matches(pattern,operand)){
        return NULL;
    }
    int value;
    sscanf(operand,"[%d]",&value);
    if (value>=cpu->memory_handler->total_size || value<0){
        return NULL;
    }

    return load(cpu->memory_handler,"DS",value);

}

void * register_indirect_addressing(CPU * cpu, const char*operand){
    /*
        Register indirect addressing treatment
        Matches operand with register indirect addressing pattern and returns a value saved in cpu's memory at position contained in required register
        Ex: [AX] -> value stored in AX-position in memory
    */ 
    char * pattern = "^\\[[A-D]X\\]$"; // Character between A and D + X in []
    if (!matches(pattern,operand)){
        return NULL;
    }
    // Treatment by substring to avoid copy errors
    char * regist = (char*)malloc(sizeof(char)*3);
    strncpy(regist,operand+1,2); 
    regist[2] = '\0';
    int *result=(int *)hashmap_get(cpu->context,regist);
    free(regist);
    if (!result) return NULL;
    if ((*(int*)result)>=cpu->memory_handler->total_size){
        return NULL;
    }
    return load(cpu->memory_handler,"DS",*(int*)result);
}

void *segment_override_addressing(CPU *cpu, const char *operand){
    /*
        Explicit segment register indirect addressing treatment
        Matches operand with pattern and returns a value saved in cpu's memory in explicit segment at position contained in required register
        MOV [ES:AX],10 -> AX-position in ES
    */ 
    char * pattern = "^\\[(D|C|S|E)S:[A-D]X\\]$"; // ((on of char from D,C,S,E)+S):(Character between A and D + X) in []
    if (!matches(pattern,operand)){
        return NULL;
    }

    char *reg1 = (char*)malloc(sizeof(char) * 3); // For "ES"
    char *reg2 = (char*)malloc(sizeof(char) * 3); // For "AX"

    strncpy(reg1, operand + 1, 2); // Skip the '[' and copy 2 chars
    reg1[2] = '\0';

    strncpy(reg2, operand + 4, 2); // Skip past '[XX:' (4 chars total)
    reg2[2] = '\0';
    int *vregist=(int *)hashmap_get(cpu->context,reg2);
    if (!vregist) return NULL;
    void * result = load(cpu->memory_handler,reg1,*vregist);
    free(reg1);
    free(reg2);
    return result;

}


CPU *setup_test_environment() {
    /*
        Setup test environment for tests in exo5.7
    */
    CPU *cpu = cpu_init(1024);
    if (!cpu) {
        printf("Error: CPU initialization failed\n");
        return NULL;
    }

    // Initialise registers with specific values
    int *ax = (int *)hashmap_get(cpu->context, "AX");
    int *bx = (int *)hashmap_get(cpu->context, "BX");
    int *cx = (int *)hashmap_get(cpu->context, "CX");
    int *dx = (int *)hashmap_get(cpu->context, "DX");

    *ax = 3;
    *bx = 6;
    *cx = 100;
    *dx = 0;

    // Créer et initialiser le segment de données
    if (!hashmap_get(cpu->memory_handler->allocated, "DS")) {
        create_segment(cpu->memory_handler, "DS", cpu->memory_handler->free_list->start, 20);

        // Initialiser le segment de données avec des valeurs de test
        for (int i = 0; i < 10; i++) {
            int *value = (int *)malloc(sizeof(int));
            *value = i * 10 + 5;  // Valeurs 5, 15, 25, 35...
            printf("value: %d ",i);
            void *succ= store(cpu->memory_handler, "DS", i, value);
            printf("store succ: %d ",*(int*)succ);
        }
    }

    printf("Test environment initialized.\n");
    return cpu;
}

void *resolve_addressing(CPU *cpu, const char *operand){
    /*
        Function applies the appropriate type of addressing to the operand
        Input:
            CPU * cpu -- initialized CPU
            char * operand -- operand to treat
        Output:
            void * pt -- pointer to required memory case
    */
    if (!operand) return NULL;
    void *pt=immediate_addressing(cpu,operand);
    if (!pt){
        pt=register_addressing(cpu, operand);
    }
    if (!pt){
        pt=memory_direct_addressing(cpu, operand);
    }
    if (!pt){
        pt=register_indirect_addressing(cpu, operand);
    }
    if (!pt){
        pt=segment_override_addressing(cpu,operand);
    }
    return pt;
}

void handle_MOV(CPU * cpu, void * src, void *dest){
    /*
        The function handles MOV operation by replacing dest value by src value
        Input:
            CPU * cpu -- initialized CPU
            void * src -- source pointer
            void * dest -- destination pointer
        Output:
            NULL
    */
    if (!src || !dest) return;
    *((int*)dest) = *((int*)src);
}

int push_value(CPU *cpu, int value){ //retourne -1 et pas 0 en cas d'erreur
    /*
        Stack push operation
        The function pushes the value on the top of stack (Stack Segment)  
        Input:
            CPU * cpu -- initialized CPU
            int value -- value to push
        Output:
            1 in case of success
            -1 in other case
    */
    int *sp=hashmap_get(cpu->context,"SP");
    if (!sp || (*sp)<0) return -1;
    void* s=store(cpu->memory_handler,"SS",*sp,int_to_point(value));
    if (!s) return -1;
    (*sp)--;
    return 1;
}

int pop_value(CPU *cpu, int* dest){ //caster le void* à l'appel?
    /*
        Stack pop operation
        The function deletes the value from the top of stack (Stack Segment)
        Then returns a value by parameter 
        Input:
            CPU * cpu -- initialized CPU
            int * dest -- pointer to memory receiveing the value
        Output:
            1 in case of success + value returned by parameter
            -1 in other case
    */
    int *sp=hashmap_get(cpu->context,"SP");
    if (*sp>=128) return -1;
    void*l=load(cpu->memory_handler,"SS",(*sp)+1);
    if (!l) return -1;
    *dest=*((int *)l);
    (*sp)++;
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

int alloc_es_segment(CPU* cpu){
    /*
        Dynamic allocation of ES segment
        The function allocates a segment ES of size AX's value according the strategy stored in register BX,
        then it changes ZF register's value to 0 in case of successful allocation
            -- for strategy types look find_free_address_strategy
        Input:
            CPU * cpu -- initialized CPU
        Output: 
            1 in case of success
            0 in other cases
    */
    int* ax=hashmap_get(cpu->context,"AX"); //Get value of AX register
    int *bx=hashmap_get(cpu->context,"BX"); //Get value of BX register
    if (!((*bx==0) || (*bx==1)|| (*bx==2) ) ){
        printf("Error alloc_es_segment : register BX not properly set to an allocating strategy. Set to 0, 1 or 2\n ");
        usleep(1300000);
     return 0;   
    } 
    int *zf=hashmap_get(cpu->context,"ZF"); //Get value of ZF register
    int start=find_free_address_strategy(cpu->memory_handler,*ax,*bx);
    if ((start)==-1 ) { // required segment not found
        *zf=1;
        return 0;
    }
    int succ=create_segment(cpu->memory_handler,"ES",start,*ax);

    
    *zf=!(succ);
    if (!succ) return 0; 
    for (int i=0;i<*ax;i++){
        store(cpu->memory_handler,"ES",i,int_to_point(0)); //constant pool? add it directly in int to point maybe
    }
    //Saving a new ES segment in context 
    int* es=hashmap_get(cpu->context,"ES");
    *es=start;

    
    return 1;


}

int free_es_segment(CPU* cpu){
    /*
        ES segment remove function
        The function clears dynamic memory allocate by a segment ES
        Input:
            CPU * cpu -- initialized CPU
        Output: 
            1 in case of success
            0 in other cases
    */
    Segment* esseg=hashmap_get(cpu->memory_handler->allocated,"ES");
    if  (esseg==NULL) return 0;

    for (int i=0;i<esseg->size;i++){
        free(load(cpu->memory_handler,"ES",i)); //constant pool? add it directly in int to point maybe
    }
    int succ=remove_segment(cpu->memory_handler->allocated,"ES"); // treats memory of segment Object
    if (succ==0) return succ;

    int* es=hashmap_get(cpu->context,"ES");
    *es=-1;

    return 1;

}














