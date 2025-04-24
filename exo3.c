#include "exo3.h"

int nb_data_occ = 0;

Instruction *parse_data_instruction(const char * line, HashMap *memory_locations){
    /*
        .DATA instructions parsing function: 
        Parses an instruction of type .DATA from the original file into instruction type for the programme
        
        Input: 
            char * line -- line of .DATA in pseudo-assembler (Ex: x DW 47)
            HashMap * memory_locations -- hashmap containing pair (variable_name <-> position_in_memory) 
        Output: Instruction * inst (Ex: inst->mnemonic = x, inst->operand1 = DW, inst->operand2 = 47)
    */
    //global counter of .DATA instructions
    if (line==NULL){
         printf("Error parse_data_instructions : no line to parse instruction from \n");
         return NULL;
    }

    if (memory_locations==NULL){
         printf("Error parse_data_instructions : no \"memory_locations\" hashmap to parse in \n");
         return NULL;
    }

    static int nb_occ = 0;
    char mnemonic[256];
    char operand1[256];
    char operand2[256];
    
    // scanning of initial line
    sscanf(line,"%s %s %s", mnemonic, operand1, operand2);
    int i = 0;
    int count = 1;
    // arrays treatment 
    while(operand2[i]!='\0'){
        if (operand2[i]==','){
            count++;
        }
        i++;
    }
    Instruction * inst = (Instruction*)malloc(sizeof(Instruction));
    inst->mnemonic = strdup(mnemonic);
    inst->operand1 = strdup(operand1);
    inst->operand2 = strdup(operand2);
    // saving position in memory
    int * value = malloc(sizeof(int));
    *value = nb_occ;
    if (hashmap_insert(memory_locations,mnemonic,value)==0){
         printf("Error parse_data_instructions : insertion in \"memory_locations\" hashmap failed \n");
         return NULL;
    }
    nb_occ += count;
    nb_data_occ = nb_occ;
    return inst;
}

Instruction *parse_code_instruction(const char *line, HashMap *labels, int code_count) {
    if (line==NULL){
         printf("Error parse_code_instructions : no line to parse instruction from \n");
         return NULL;
    }
    if (labels==NULL){
         printf("Error parse_code_instructions : no \"labels\" to parse in \n");
         return NULL;
    }
    
    char buffer[256];
    strncpy(buffer, line, 255);
    buffer[255] = '\0'; // Ensure null termination

    // Initialize pointers for label, mnemonic, and operands
    char *label = NULL;
    char *mnemonic = NULL;
    char *operand1 = NULL;
    char *operand2 = NULL;

    // Get the first token (could be a label or mnemonic)
    char *token = strtok(buffer, " ");
    if (!token) return NULL;

    // Check if the token contains a colon (i.e., it's a label)
    char *colon = strchr(token, ':');
    if (colon) {
        // Check if it's NOT part of a [bracketed] operand like [ES:AX]
        char *open_bracket = strchr(token, '[');
        char *close_bracket = strchr(token, ']');

        int colon_inside_brackets = (open_bracket && close_bracket &&
                                    open_bracket < colon && colon < close_bracket);

        if (!colon_inside_brackets) {
            *colon = '\0';
            label = token;
            token = strtok(NULL, " ");
        }
    }

    // If there's no mnemonic after label (or to begin with), return NULL
    if (!token) return NULL;
    mnemonic = token; // Save the mnemonic (instruction name)

    // Try to get first operand (if any)
    token = strtok(NULL, " ,");
    if (token) {
        operand1 = token; // Save first operand

        // Try to get second operand (if any)
        token = strtok(NULL, " ,");
        if (token) {
            operand2 = token; // Save second operand
        }
    }

    // Allocate memory for the Instruction structure
    Instruction *inst = (Instruction *)malloc(sizeof(Instruction));
    inst->mnemonic = strdup(mnemonic);                // Copy mnemonic string
    inst->operand1 = operand1 ? strdup(operand1) : NULL; // Copy operand1 if it exists
    inst->operand2 = operand2 ? strdup(operand2) : NULL; // Copy operand2 if it exists

    // If a label was found, insert it into the label hashmap with current code count
    if (label) {
        int *code_c = malloc(sizeof(int)); // Allocate memory for code count
        *code_c = code_count;              // Store the current instruction index
        hashmap_insert(labels, label, code_c); // Add label to the hashmap
    }

    // Return the constructed instruction
    return inst;
}




ParserResult *parse(const char *filename){
    if (filename==NULL){
        printf("Error parse : no file name to parse file  \n");
        return NULL;
    }

    int len = strlen(filename);
    if (!(len >= 4 && strcmp(filename + len - 4, ".txt") == 0)){
        printf("Error parse : File name in wrong format. Enter a file name with .txt extension \n");
        return NULL;
    }

    //Parser creation
    ParserResult *parser=(ParserResult *)malloc(sizeof(ParserResult));
    assert(parser);
    parser->data_instructions=(Instruction **)malloc(sizeof(Instruction *)*NBMININST);
    assert(parser->data_instructions);
    parser->code_instructions=(Instruction **)malloc(sizeof(Instruction *)*NBMININST);
    assert(parser->code_instructions);
    parser->memory_locations=hashmap_create();
    if (parser->memory_locations==NULL){
        printf("Error parse : Failed to create hashmap \"memory_locations\" \n");
        return NULL;
    }
    parser->labels=hashmap_create();
    if (parser->memory_locations==NULL){
        printf("Error parse : Failed to create hashmap \"labels\" \n");
        return NULL;
    }
    
    FILE *F=fopen(filename,"r");
    if (F==NULL){
        printf("Error parse : file not found / error opening file \n");
        return NULL;
    }
    char buffer[256];
    char* lect=fgets(buffer,256,F);
    assert(lect);
    if( buffer[strlen(buffer)-1] == '\n'){
        buffer[strlen(buffer)-1] = '\0';
    }
    assert(strcmp(buffer,".DATA")==0);
   
    int tailledata=NBMININST;
    // .DATA instruction counter
    int nbdata=0;
    lect=fgets(buffer,256,F);
    if( buffer[strlen(buffer)-1] == '\n'){
        buffer[strlen(buffer)-1] = '\0';
    }
    while((strcmp(buffer,".CODE")!=0) && (lect!=NULL)){
        //To many instructions
        if (nbdata>=tailledata){
            // double table size
            tailledata*=2;
            Instruction **tmp = realloc(parser->data_instructions, tailledata);
            assert(tmp);
            parser->data_instructions= tmp;
        }
        Instruction *inst= parse_data_instruction(buffer,parser->memory_locations);
        if (inst==NULL){
        printf("Error parse : Failed to parse a data instruction. Aborting. \n");
        return NULL;
        }
        parser->data_instructions[nbdata]=inst;
        nbdata++;

        lect=fgets(buffer,256,F);
        if( buffer[strlen(buffer)-1] == '\n'){
            buffer[strlen(buffer)-1] = '\0';
        }
    }
    parser->data_count=nbdata;
    
    // .CODE part exists?

    assert(lect);

    //.CODE instructions table  
    int taillecode=NBMININST;
    // .CODE instructions counter
    int nbcode=0;
    lect=fgets(buffer,256,F);
    if( buffer[strlen(buffer)-1] == '\n'){
        buffer[strlen(buffer)-1] = '\0';
    }
    while(lect!=NULL && strcmp(lect,"\0")!=0){
        //To many instructions
        if (nbcode>=taillecode){
            // double table size
            taillecode*=2;
            Instruction **tmp = realloc(parser->code_instructions, taillecode);
            assert(tmp);
            parser->code_instructions= tmp;
        }
        Instruction *inst= parse_code_instruction(buffer,parser->labels, nbcode);
        if (inst==NULL){
        printf("Error parse : Failed to parse a code instruction. Aborting. \n");
        return NULL;
        }
         parser->code_instructions[nbcode]=inst;
         nbcode++;

        lect=fgets(buffer,256,F);
        if( buffer[strlen(buffer)-1] == '\n'){
            buffer[strlen(buffer)-1] = '\0';
        }
    }
    parser->code_count=nbcode;
    fclose(F);
    return parser;
}


void free_parser_result(ParserResult *parser){
    if (parser==NULL){
        printf("Error free_parser_result : no parser to free \n");
        return ;
        }
    // for(int i = 0; i<parser->data_count;i++){
    //     Instruction* inst = parser->data_instructions[i];
    //     free(inst->mnemonic);
    //     free(inst->operand1);
    //     if (inst->operand2) free(inst->operand2);
    //     free(inst);
    // }
    free(parser->data_instructions);
    free(parser->code_instructions);
    hashmap_destroy(parser->memory_locations);
    hashmap_destroy(parser->labels);
    free(parser);
    parser = NULL;
}


void print_instruction(Instruction *inst){
    if (inst==NULL){
        printf("Error print_instruction : no instruction found\n");
        return;
    }
    printf("%s ", inst->mnemonic);
    if (inst->operand1){
        printf("%s ",inst->operand1);
        if (inst->operand2){
            printf("%s ",inst->operand2);
    }
    }
    printf(" ");
    fflush(stdout);

}


void parser_show(ParserResult * parser){
    if (parser==NULL){
        printf("Error parser_show: no parser found\n");
        return;
    }
    printf("====== PARSER CONTENT ======\n\n");
    printf("______ .DATA instructions ______\n\n");
    for (int i = 0; i<parser->data_count;i++){
        print_instruction(parser->data_instructions[i]);
        printf("\n");
    }
    printf("\n --- memory locations ---\n");
    print_hashmap_int(parser->memory_locations);
    printf("\n______ .CODE instructions ______\n\n");
    for (int i = 0; i<parser->code_count;i++){
        print_instruction(parser->code_instructions[i]);
        printf("\n");
    }
    printf("\n--- labels ---\n\n");
    print_hashmap_int(parser->labels);

    printf("====== END OF PARSER CONTENT ======\n\n");
}

void free_instruction(Instruction *inst){
    if (inst==NULL){
        printf("Error free_instruction : no instruction found\n");
        return;
    }
    print_instruction(inst);
    free(inst->mnemonic);
    if (inst->operand1) free(inst->operand1);
    if (inst->operand2) free(inst->operand2);   
    free(inst);
}

void free_memory_handler(MemoryHandler * handler){
    if (handler==NULL){
        printf("Error free_memory_handler : no handler to free\n");
        return;
    }
    /*
        Delete MemoryHandler:
        The function clears the memory space occupied by the structure  
        Input: 
            MemoryHandler * handler
        Output: NULL
    */

    // Data segment clearing
    Segment* data_segment = hashmap_get(handler->allocated, "DS");
    if (data_segment) {
        for (int i = 0; i < data_segment->size; i++) {
            free(load(handler,"DS",i));
        }
    }

    //Code segment clearing
    Segment* code_segment = hashmap_get(handler->allocated, "CS");
    if (code_segment) {
        for (int i = 0; i < code_segment->size; i++) {
            // .CODE instructions clearing
            free_instruction(load(handler,"CS",i));
        }
    }
    //Extra-segment clearing
    Segment* extra_segment = hashmap_get(handler->allocated, "ES");
    if (extra_segment) {
        for (int i = 0; i < extra_segment->size; i++) {
            free(load(handler,"ES",i));
        }
    }
    //Stack-segment clearing
    Segment* stack_segment = hashmap_get(handler->allocated, "SS");
	if (stack_segment) {
		for (int i = 0; i < stack_segment->size; i++) {
			free(load(handler,"SS",i));
		}
	}

    hashmap_destroy(handler->allocated);

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
        printf("Segmentation fault store in memory tab: value out of bounds\n");
        return NULL;
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
        printf("Segmentation fault load in memory tab: value out of bounds\n");
        return NULL;
    }
    return (handler->memory[seg->start+pos]);
}
