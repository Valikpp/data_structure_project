#include "exo1.h"

unsigned long simple_hash(const char *str){
    /*
        Hashing function for string keys based on ASCII encoding value 
        
        Input: string str
        Output: int index
    */
    int i = 0;
    int cle = 0;
    // Sum of ASCII values
    while(str[i]!='\0'){
        //since cle is an int, we retrieve the ASCII number of each character
        cle+= str[i];
        i++;
    }
    // Golden ratio
    float A = (sqrt(5)-1)/(2);
    //cast int to obtain the lower integer part
    return (int)( TABLE_SIZE * ( cle*A - (int)(cle*A) ));
}

HashMap *hashmap_create(){
    /*
        Hashmap creation function: allocates dynamicly a hashmap of size TABLE_SIZE,
        Initialises all cases by NULL
        
        Input: NULL
        Output: Hashmap* new_hashmap 
    */
    HashMap *nv=(HashMap *)malloc(sizeof(HashMap));
    assert(nv);
    nv->size=TABLE_SIZE;
    nv->table=(HashEntry*)malloc(sizeof(HashEntry)*(nv->size));
    assert(nv->table);
    for (int i=0;i<nv->size;i++){
        nv->table[i].key = NULL;
        nv->table[i].value = NULL;
    }
    return nv;
}

int hashmap_insert(HashMap *map, const char* key, void* value){
    /*
        Hashmap insertion function: Assigns a data pointer to the cell with the key
        Assignment takes place according to the linear probing method using the following formula: h<k,i> = hash(k)+i 
        
        Input: 
            HashMap *map -- pointer to initialized hashmap struct
            const string key -- key of inserted data
            void * data -- pointer to indefined type value
            PREREQUISITE: void * data is a DYNAMICLY allocated pointer () 
        Output: 0 in case the table is full
                i+1 in case of successful assignment to cell i
    */

    // Hash(key)
    int hash=simple_hash(key);
    int i = 0;

    // Iterator restricted to TABLE_SIZE
    int indice = (hash+i)%TABLE_SIZE;
    
    // Pointer to key of table[index] cell
    void * cle_courante = (void*)(map->table)[indice].key;
    
    // Scan until you find an empty square
    while ((cle_courante)&&(cle_courante!=TOMBSTONE)){   
        i++;
        indice = (hash+i)%TABLE_SIZE;
        cle_courante = map->table[indice].key;
        // We made a complete tour
        if(indice==hash){
            return 0;
        }
    }
    // Value assignment
    map->table[indice].key = strdup(key);
    map->table[indice].value = value;
    return i+1;
}
void * hashmap_get(HashMap *map, const char * key){
    /*
        Hash Table Search:  
            The function searches for a cell with a specified key and returns its value
        Input: 
            HashMap *map -- pointer to initialized hashmap struct
            const string key -- key of removed data 
        Output: NULL if key does not exist in hashmap
                void * data in case of a successful finding
    */
    int hash=simple_hash(key);
    int i = 0;
    int indice = (hash+i)%TABLE_SIZE;
    char * cle_courante = map->table[indice].key;
    //on parcours jusqua trouver une case vide
    
    while (cle_courante){
        if(strcmp(cle_courante,key)==0){
            return (map->table[indice]).value;
        }
        i++;
        indice = (hash+i)%TABLE_SIZE;
        cle_courante = map->table[indice].key;
        //We made a complete tour
        if(indice==hash){
            return NULL;
        }
    }
    return NULL;
}

int hashmap_remove(HashMap *map,const char *key){
    /*
        Hashmap insertion function: 
        Delete value function: Deletes the value with the specified key (without clearing memory)
        
        Input: 
            HashMap *map -- pointer to initialized hashmap struct
            const string key -- key of removed data 
        Output: 0 if key does not exist in hashmap
                1 in case of successful deletion
    */
    int hash=simple_hash(key);
    int i = 0;
    int indice = (hash+i)%TABLE_SIZE;
    char * cle_courante = map->table[indice].key;
    //Search until we find a cell with the specified key
    while (cle_courante){
        // Specified key found
        if(strcmp(cle_courante,key)==0){
            free(map->table[indice].key);
            (map->table[indice]).key = TOMBSTONE;
            map->table[indice].value = NULL;
            return 1;
        }
        i++;
        indice = (hash+i)%TABLE_SIZE;
        cle_courante = map->table[indice].key;
        //We made a complete tour
        if(indice==hash){
            return 0;
        }
    }
    return 0;
}

void hashmap_destroy(HashMap *map){
    /*
        Function of clearing dynamic memory of the whole table with clearing of saved pointers
        Input: HashMap* map
        Output: NULL
    */
    for(int i = 0;i<TABLE_SIZE;i++){
        //If cell is not empty
        if((map->table[i]).key!=TOMBSTONE){
            free((map->table[i]).key);
            free((map->table[i]).value);
        //Cell is empty
        } else {
            map->table[i].key = NULL;
        }
    }
    free(map->table);
    free(map);
}

void hashmap_show_keys(HashMap *map){
    /*
        Utility function showing hashmap keys
    */
    for(int i = 0;i<TABLE_SIZE;i++){
        if(((map->table[i]).key!=TOMBSTONE) && ((map->table[i]).key!=NULL)){
            printf("%s\n",map->table[i].key);
        }
    }
}
void hashmap_show_pairs_Integer(HashMap *map){
    /*
        Utility function showing hashmap with integer values
        ATTENTION: Other type of values cause Segmentation fault!
    */
    for(int i = 0;i<TABLE_SIZE;i++){
        if(((map->table[i]).key!=TOMBSTONE) && ((map->table[i]).key!=NULL)){
            printf("%s --> %d\n",map->table[i].key,*(int *)(map->table[i].value));
        }
    }
}

int* int_to_point(int value){
    /* Transforms quickly int to dynamic int pointer*/
    int * val = malloc(sizeof(int)); 
    *val = value;
    return val;
}


    //map->table[indice].value = malloc(sizeof(value));

    // On recopie la valeur dans la table de hashage pour eviter les modification de valeur a l'interieur de la table
    /* SINON 
    value = a
    hashmap_insert(key,value);
    value = b
    hashmap_get(key) -> b 
    PAS LOGIQUE POUR UTILISATION, on veut recevoir "a"
    */
    //memcpy(map->table[indice].value,value,sizeof(value));