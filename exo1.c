#include "exo1.h"

unsigned long simple_hash(const char *str){
    int i = 0;
    int cle = 0;
    while(str[i]!='\0'){
        //puisque cle est un int, on récupère le numero ASCII de chaque caractère
        cle+= str[i];
        i++;
    }
    float A = (sqrt(5)-1)/(2);
    //cast int pour obtenir la partie entiere inferieure
    return (int)( TABLE_SIZE * ( cle*A - (int)(cle*A) ));
}

HashMap *hashmap_create(){
    HashMap *nv=(HashMap *)malloc(sizeof(HashMap));
    assert(nv);
    nv->size=TABLE_SIZE;
    nv->table=(HashEntry*)malloc(sizeof(HashEntry)*(nv->size));
    assert(nv->table);
    for (int i=0;i<nv->size;i++){
        nv->table[i].key = NULL;
        nv->table[i].value = NULL;
    }
}

int hashmap_insert(HashMap *map, const char* key, void* value){
    int hash=simple_hash(key);
    int i = 0;
    int indice = (hash+i)%TABLE_SIZE;
    void * cle_courante = map->table[indice].key;
    //on parcours jusqua trouver une case vide
    while ((cle_courante)&&(cle_courante!=TOMBSTONE)){
        i++;
        indice = (hash+i)%TABLE_SIZE;
        cle_courante = map->table[indice].key;
        //On a fait un tour complet
        if(indice==hash){
            return 0;
        }
    }
    map->table[indice].value = value;
    return i;
}
void * hashmap_get(HashMap *map, const char * key){
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
        //On a fait un tour complet
        if(indice==hash){
            return NULL;
        }
    }
    return NULL;
}

int hashmap_remove(HashMap *map,const char *key){
    int hash=simple_hash(key);
    int i = 0;
    int indice = (hash+i)%TABLE_SIZE;
    char * cle_courante = map->table[indice].key;
    //on parcours jusqua trouver une case vide
    while (cle_courante){
        if(strcmp(cle_courante,key)==0){
            free((map->table[indice]).value);
            free((map->table[indice]).key);
            (map->table[indice]).key = TOMBSTONE;
            return 1;
        }
        i++;
        indice = (hash+i)%TABLE_SIZE;
        cle_courante = map->table[indice].key;
        //On a fait un tour complet
        if(indice==hash){
            return 0;
        }
    }
    return 0;
}

void hashmap_destroy(HashMap *map){
    for(int i = 0;i<TABLE_SIZE;i++){
        free((map->table[i]).key);
        free((map->table[i]).value);
    }
    free(map->table);
    free(map);
}

