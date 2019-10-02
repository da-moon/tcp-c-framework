#ifndef HASHMAP
#define HASHMAP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define HMAP_KEY_TYPE char *
#define HMAP_VAL_TYPE void *
#define HMAP_KEY_NOT_EXISTS 1

typedef HMAP_KEY_TYPE hkey;
typedef HMAP_VAL_TYPE hval;

typedef struct hash_t Hash;
struct hash_t
{
    hkey key;
    hval val;

    Hash *previous;
    Hash *next;
};

typedef struct
{
    Hash *first;
    Hash *last;
} HashMap;

HashMap hmap_init();
hval hmap_get(HashMap map, hkey key);
int hmap_contains(HashMap map, hkey key);
void hmap_set(HashMap *map, hkey key, hval val);
int hmap_remove(HashMap *map, hkey key);
void hmap_loop(HashMap *map, void (*cb)(Hash*, void*), void* args);

#endif
