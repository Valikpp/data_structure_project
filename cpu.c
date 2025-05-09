#include "cpu.h"

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

    if (cpu==NULL) { 
        printf("Error allocate_variables : CPU not found \n"); 
        return ; 
    }
    if (data_instructions==NULL) { 
        printf("Error allocate_variables : \"data_instructions\" table not found \n"); 
        return ; 
    }
    if (data_count==0) { 
        printf("Error allocate_variables : data_count is null, no variables to allocate \n"); 
        return ; 
    }
    int start = find_free_address_strategy(cpu->memory_handler,nb_data_occ,0);
    int succ=create_segment(cpu->memory_handler,"DS",start,nb_data_occ);
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
                if (store(cpu->memory_handler,"DS",pos,int_to_point(value))==0) { 
                printf("Error allocate_variables : storing in data segment unsuccessful at position %d. Aborting.\n", pos); 
                return ; 
                }
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
        if (store(cpu->memory_handler,"DS",pos,int_to_point(value))==0) { 
        printf("Error allocate_variables : storing in data segment unsuccessful at position %d. Aborting.\n", pos); 
        return ; 
        }
        pos++;
        free_instruction(data_instructions[i]);
        data_instructions[i]=NULL;
    }
}

void print_data_segment(CPU * cpu){
    if (cpu==NULL) { 
        printf("Error print_data_segment : CPU not found \n"); 
        return ; 
    }
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
   if (cpu==NULL) { 
        printf("Error immediate_adressing : CPU not found \n"); 
        return NULL ; 
    }
    if (operand==NULL) { 
        printf("Error immediate_adressing : operand not found \n"); 
        return NULL ; 
    }
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
    if (hashmap_insert(cpu->constant_pool,operand,result)==0){
        printf("Error immediate_adressing : Failure to insert constant into hashmap \"constant pool\" \n"); 
        return NULL ;
    }
    return (void*)result;  
}

void *register_addressing(CPU * cpu, const char*operand){
    /*
        Register addressing treatment
        Matches operand with register addressing pattern and returns a value of required register
    */ 
    if (cpu==NULL) { 
        printf("Error register_adressing : CPU not found \n"); 
        return NULL ; 
    }
    if (operand==NULL) { 
        printf("Error register_adressing : operand not found \n"); 
        return NULL ; 
    }
    char * pattern = "^[A-D]X$"; // Character between A and D + X
    if (!matches(pattern,operand)){
        return NULL;
    }
    int *result=(int *)hashmap_get(cpu->context,operand);
     if (result==NULL) { 
        printf("Error register_adressing : CPU register not found in hashmap \"context\" \n"); 
        return NULL ; 
    }
    

    return (void *) result;
}

void *memory_direct_addressing(CPU * cpu, const char*operand){
    /*
        Memory direct addressing treatment
        Matches operand with memory direct addressing pattern and returns a value saved in cpu's memory at required position
    */ 
    if (cpu==NULL) { 
        printf("Error memory_direct_adressing : CPU not found \n"); 
        return NULL ; 
    }
    if (operand==NULL) { 
        printf("Error memory_direct_adressing : operand not found \n"); 
        return NULL ; 
    }
    char * pattern = "^\\[[0-9]+\\]$"; //integer in []
    if (!matches(pattern,operand)){
        return NULL;
    }
    int value;
    sscanf(operand,"[%d]",&value);
    
    if (load(cpu->memory_handler,"DS",value)==NULL){
        printf("Error memory_direct_adressing : address in brackets out of bounds \n"); 
        return NULL ; 
    }

    return load(cpu->memory_handler,"DS",value);

}

void * register_indirect_addressing(CPU * cpu, const char*operand){
    /*
        Register indirect addressing treatment
        Matches operand with register indirect addressing pattern and returns a value saved in cpu's memory at position contained in required register
        Ex: [AX] -> value stored in AX-position in memory
    */ 
   if (cpu==NULL) { 
        printf("Error register_direct_adressing : CPU not found \n"); 
        return NULL ; 
    }
    if (operand==NULL) { 
        printf("Error register_direct_adressing : operand not found \n"); 
        return NULL ; 
    }
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
    if (load(cpu->memory_handler,"DS",*(int*)result)==NULL){
        printf("Error register_indirect_adressing : address in register out of bounds \n"); 
        return NULL ; 
    }
    return load(cpu->memory_handler,"DS",*(int*)result);
}

void *segment_override_addressing(CPU *cpu, const char *operand){
    /*
        Explicit segment register indirect addressing treatment
        Matches operand with pattern and returns a value saved in cpu's memory in explicit segment at position contained in required register
        MOV [ES:AX],10 -> AX-position in ES
    */ 
    if (cpu==NULL) { 
        printf("Error segment_override_adressing : CPU not found \n"); 
        return NULL ; 
    }
    if (operand==NULL) { 
        printf("Error segment_override_adressing : operand not found \n"); 
        return NULL ; 
    }
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
    if (!vregist){
        printf("Error segment_override_adressing : register after colon not found \n"); 
        return NULL;
    }
    void * result = load(cpu->memory_handler,reg1,*vregist);
    if (result==NULL) { 
        printf("Error segment_override_adressing : failed to load, segment non existant or address in register out of bounds \n"); 
        return NULL ; 
    }
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
    if (operand==NULL) { 
        return NULL ; 
    }
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
    if (pt==NULL) { 
        printf("Error resolve_adressing : Failed to resolve addressing , Wrong operand format for operand %s \n", operand); 
        return NULL ; 
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
    */if (cpu==NULL) { 
        printf("Error handle_mov : no CPU found  \n,"); 
        return  ; 
    }
    if (src==NULL) { 
        printf("Error handle_mov : no source found\n"); 
        return ; 
    }
    if (dest==NULL) { 
        printf("Error handle_mov : no destination found\n"); 
        return  ; 
    }
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
    if (cpu==NULL) { 
        printf("Error push_value : no CPU found\n"); 
        return  -1; 
    }
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
    if (dest==NULL) { 
        printf("Error push_value : no destination found\n"); 
        return  -1; 
    }
    if (cpu==NULL) { 
        printf("Error push_value : no CPU found\n"); 
        return  -1; 
    }
    int *sp=hashmap_get(cpu->context,"SP");
    if (*sp>=128) return -1;
    void*l=load(cpu->memory_handler,"SS",(*sp)+1);
    if (!l) return -1;
    *dest=*((int *)l);
    (*sp)++;
    return 1;
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
    if (ax==NULL ){
        printf("Error alloc_es_segment :  register AX not found \n ");
     return 0;   
    } 
    int *bx=hashmap_get(cpu->context,"BX"); //Get value of BX register
    if (bx==NULL ){
        printf("Error alloc_es_segment :  register BX not found \n ");
     return 0;   
    } 
    if (!((*bx==0) || (*bx==1)|| (*bx==2) ) ){
        printf("Error alloc_es_segment : register BX not properly set to an allocating strategy. Set to 0, 1 or 2\n ");
        usleep(1300000);
     return 0;   
    } 
    int *zf=hashmap_get(cpu->context,"ZF"); //Get value of ZF register
    if (zf==NULL ){
        printf("Error alloc_es_segment : register ZF not found \n ");
     return 0;   
    } 
    int start=find_free_address_strategy(cpu->memory_handler,*ax,*bx);
    if (start==-1 ) { // required segment not found
        *zf=1;
        printf("Error alloc_es_segment : no segment found to allocate Extra Segment \n ");
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
    if (es==NULL ){
        printf("Error alloc_es_segment :  register ES not found \n ");
     return 0;   
    } 
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
   if (cpu==NULL) { 
        printf("Error free_es_segment : no CPU found\n"); 
        return  -1; 
    }
    Segment* esseg=hashmap_get(cpu->memory_handler->allocated,"ES");
    if  (esseg==NULL) {
        printf("Error free_es_segment : Extra Segment not found to free \n"); 
    return 0;
    }

    for (int i=0;i<esseg->size;i++){
        free(load(cpu->memory_handler,"ES",i)); 
    }
    int succ=remove_segment(cpu->memory_handler,"ES"); 
    if (succ==0) return succ;

    int* es=hashmap_get(cpu->context,"ES");
    *es=-1;

    return 1;

}














