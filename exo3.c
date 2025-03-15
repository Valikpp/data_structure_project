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
    printf("%d\n",nb_occ);
    nb_occ += count;
    return inst;
}

Instruction *parse_code_instruction(const char *line,HashMap *labels, int code_count){
    int i=0;
    int countesp=1;
    int countoper=1;
    while (line[i]!='\0'){
        if (line[i]==' '){
            countesp++;
        }
        if (line[i]==','){
            countoper++;
        }

        i++;
    }
    char mnemonic[256];
    char operand1[256];
    char operand2[256];

    if (countesp>2){
        char label[256];
        if (countoper>1){
            sscanf(line,"%s %s %s,%s",label, mnemonic, operand1, operand2);
        } else {
            sscanf(line,"%s %s %s",label, mnemonic, operand1);
        }
        int *code_c = malloc(sizeof(int));
        *code_c = code_count;
        hashmap_insert(labels,strdup(label),code_c);
    }else{
        if (countoper>1){
            sscanf(line,"%s %s,%s", mnemonic, operand1, operand2);
        } else {
            sscanf(line,"%s %s", mnemonic, operand1);
        }
    }
    Instruction * inst = (Instruction*)malloc(sizeof(Instruction));
    inst->mnemonic = strdup(mnemonic);
    inst->operand1 = strdup(operand1);
    if (countoper>1){
        inst->operand2 =strdup(operand2);
    } else {
        inst->operand2 = NULL;
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
    int lect=fgets(buffer,256,F);
    assert(lect && strcmp(buffer,".DATA")==0);

    //Taille du tableau contenant les instructions .DATA   
    int tailledata=NBMININST;
    //Compteur des instructions dans .DATA
    int nbdata=0;
    lect=fgets(buffer,256,F);
    while(strcmp(buffer,".CODE")!=0 && lect!=NULL){
        //Si le nombre d'instruction dépasse la taille du tableau
        if (nbdata>=tailledata){
            //On double la taille du tableau
            tailledata*=2;
            Instruction *tmp = realloc(parser->data_instructions, tailledata);
            assert(tmp);
            parser->data_instructions= tmp;
        }
        Instruction *inst= parse_data_instruction(buffer,parser->memory_locations);
         parser->data_instructions[nbdata]=inst;
         nbdata++;

        lect=fgets(buffer,256,F);
    }
    parser->data_count=nbdata;
    //il existe une section code ??
    assert(lect);

    //Taille du tableau contenant les instructions .CODE  
    int taillecode=NBMININST;
    //Compteur des instructions dans .CODE
    int nbcode=0;
    lect=fgets(buffer,256,F);
    while(lect!=NULL){
        //Si le nombre d'instruction dépasse la taille du tableau
        if (nbcode>=taillecode){
            //On double la taille du tableau
            taillecode*=2;
            Instruction *tmp = realloc(parser->code_instructions, taillecode);
            assert(tmp);
            parser->code_instructions= tmp;
        }
        Instruction *inst= parse_code_instruction(buffer,parser->labels, nbcode);
         parser->code_instructions[nbcode]=inst;
         nbcode++;

        lect=fgets(buffer,256,F);
    }
    parser->code_count=nbcode;

    return parser;
}


