#ifndef EXO1
#define EXO1
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#define TABLE_SIZE 128
#define TOMBSTONE ((void*)-1)

typedef struct hashentry{
    char *key;
    void *value;
}HashEntry;

typedef struct hashmap{
    int size;
    HashEntry* table;
}HashMap;


unsigned long simple_hash(const char *str);
HashMap *hashmap_create();

int hashmap_insert(HashMap *map, const char* key, void* value);
void * hashmap_get(HashMap *map, const char * key);
int hashmap_remove(HashMap *map,const char *key);
void hashmap_destroy(HashMap *map);
void hashmap_show_keys(HashMap *map);

#endif




