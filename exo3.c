#include "exo3.h"

Instruction *parse_data_instruction(const char * line, HashMap *memory_locations){
    static int nb_occ = 0;
    char mnemonic[256];
    char operand1[256];
    char operand2[256];
    sscanf(line,"%s %s %s", mnemonic, operand1, operand2);
    int i = 0;
    int count = 1;
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
    int * value = malloc(sizeof(int));
    *value = nb_occ;
    hashmap_insert(memory_locations,mnemonic,value);
    nb_occ += count;
    return inst;
}

Instruction *parse_code_instruction(const char *line, HashMap *labels, int code_count) {
    char buffer[256];
    strncpy(buffer, line, 255);
    buffer[255] = '\0';

    char *label = NULL;
    char *mnemonic = NULL;
    char *operand1 = NULL;
    char *operand2 = NULL;

    char *token = strtok(buffer, " ");
    if (!token) return NULL;

    char *colon = strchr(token, ':');
    if (colon) {
        *colon = '\0';
        label = token;
        token = strtok(NULL, " ");
    }

    if (!token) return NULL;
    mnemonic = token;

    token = strtok(NULL, " ,");
    if (token) {
        operand1 = token;
        token = strtok(NULL, " ,");
        if (token) {
            operand2 = token;
        }
    }

    Instruction *inst = (Instruction *)malloc(sizeof(Instruction));
    inst->mnemonic = strdup(mnemonic);
    inst->operand1 = operand1 ? strdup(operand1) : NULL;
    inst->operand2 = operand2 ? strdup(operand2) : NULL;

    if (label) {
        int *code_c = malloc(sizeof(int));
        *code_c = code_count;
        hashmap_insert(labels, label, code_c);
    }

    return inst;
}




ParserResult *parse(const char *filename){
    //On cree un parser
    ParserResult *parser=(ParserResult *)malloc(sizeof(ParserResult));
    assert(parser);
    parser->data_instructions=(Instruction **)malloc(sizeof(Instruction *)*NBMININST);
    parser->code_instructions=(Instruction **)malloc(sizeof(Instruction *)*NBMININST);
    parser->memory_locations=hashmap_create();
    parser->labels=hashmap_create();
    
   

    FILE *F=fopen(filename,"r");
    char buffer[256];
    char* lect=fgets(buffer,256,F);
    assert(lect);
    if( buffer[strlen(buffer)-1] == '\n'){
        buffer[strlen(buffer)-1] = '\0';
    }
    assert(strcmp(buffer,".DATA")==0);

    //Taille du tableau contenant les instructions .DATA   
    int tailledata=NBMININST;
    //Compteur des instructions dans .DATA
    int nbdata=0;
    lect=fgets(buffer,256,F);
    if( buffer[strlen(buffer)-1] == '\n'){
        buffer[strlen(buffer)-1] = '\0';
    }
    while((strcmp(buffer,".CODE")!=0) && (lect!=NULL)){
        //Si le nombre d'instruction dépasse la taille du tableau
        if (nbdata>=tailledata){
            //On double la taille du tableau
            tailledata*=2;
            Instruction **tmp = realloc(parser->data_instructions, tailledata);
            assert(tmp);
            parser->data_instructions= tmp;
        }
        Instruction *inst= parse_data_instruction(buffer,parser->memory_locations);
        parser->data_instructions[nbdata]=inst;
        nbdata++;

        lect=fgets(buffer,256,F);
        if( buffer[strlen(buffer)-1] == '\n'){
            buffer[strlen(buffer)-1] = '\0';
        }
    }
    parser->data_count=nbdata;
    //il existe une section code ??

    assert(lect);

    //Taille du tableau contenant les instructions .CODE  
    int taillecode=NBMININST;
    //Compteur des instructions dans .CODE
    int nbcode=0;
    lect=fgets(buffer,256,F);
    if( buffer[strlen(buffer)-1] == '\n'){
        buffer[strlen(buffer)-1] = '\0';
    }
    while(lect!=NULL){
        //Si le nombre d'instruction dépasse la taille du tableau
        if (nbcode>=taillecode){
            //On double la taille du tableau
            taillecode*=2;
            Instruction **tmp = realloc(parser->code_instructions, taillecode);
            assert(tmp);
            parser->code_instructions= tmp;
        }
        Instruction *inst= parse_code_instruction(buffer,parser->labels, nbcode);
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


void free_parser_result(ParserResult *result){
    for(int i = 0; i<result->data_count;i++){
        Instruction* inst = result->data_instructions[i];
        free(inst->mnemonic);
        free(inst->operand1);
        if (inst->operand2) free(inst->operand2);
        free(inst);
    }
    free(result->data_instructions);
    for(int i = 0; i<result->code_count;i++){
        Instruction* inst = result->code_instructions[i];
        free(inst->mnemonic);
        free(inst->operand1);
        if (inst->operand2) free(inst->operand2);
        free(inst);
    }
    free(result->code_instructions);
    hashmap_destroy(result->memory_locations);
    hashmap_destroy(result->labels);
    free(result);
}



void instruction_show(Instruction* inst){
    printf("mnemonic = %s | operand1 = %s | operand2 = %s\n",inst->mnemonic,inst->operand1, (inst->operand2)? inst->operand2 : "null");
}

void parser_show(ParserResult * parser){
    printf("====== Parser content ======\n");
    printf("______ .DATA instructions ______\n");
    for (int i = 0; i<parser->data_count;i++){
        instruction_show(parser->data_instructions[i]);
    }
    printf("--- memory locations ---\n");
    hashmap_show_pairs_Integer(parser->memory_locations);
    printf("\n______ .CODE instructions ______\n");
    for (int i = 0; i<parser->code_count;i++){
        instruction_show(parser->code_instructions[i]);
    }
    printf("--- labels ---\n");
    hashmap_show_pairs_Integer(parser->labels);

    printf("====== END of Parser content ======\n");
}


