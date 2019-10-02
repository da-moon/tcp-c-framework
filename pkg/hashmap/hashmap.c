#include "hashmap.h"

HashMap hmap_init()
{
    HashMap map;

    map.first = NULL;
    map.last = NULL;

    return map;
}

Hash * hmap_get_hash(HashMap map, hkey key)
{
    Hash *row = map.first;

    while (row != NULL)
    {
        if (strcmp(row->key, key) == 0)
        {
            return row;
        }

        row = row->next;
    }

    return NULL;
}

void * hmap_get(HashMap map, hkey key)
{
    Hash *row = hmap_get_hash(map, key);

    if (row == NULL)
    {
        return NULL;
    }

    return row->val;
}

int hmap_contains(HashMap map, hkey key)
{
    return hmap_get(map, key) != NULL;
}

void hmap_set(HashMap *map, hkey key, hval val)
{
    Hash *row;

    row = hmap_get_hash(*map, key);

    if (row != NULL)
    {
        row->val = val;
    }
    else
    {
        row = (Hash *)malloc(sizeof(Hash));
        row->key = key;
        row->val = val;
        row->previous = map->last;
        row->next = NULL;

        if (map->first == NULL)
        {
            map->first = row;
        }

        if (map->last != NULL)
        {
            map->last->next = row;
        }

        map->last = row;
    }
}

int hmap_remove(HashMap *map, hkey key)
{
    Hash *row = hmap_get_hash(*map, key);

    if (row == NULL)
    {
        return HMAP_KEY_NOT_EXISTS;
    }

    if (row->previous != NULL)
    {
        row->previous->next = row->next;

        if (map->last == row)
        {
            map->last = row->previous;
        }
    }

    if (row->next != NULL)
    {
        row->next->previous = row->previous;

        if (map->first == row)
        {
            map->first = row->next;
        }
    }

    free(row);

    return 0;
}

void hmap_loop(HashMap *map, void (*cb)(Hash*, void*), void* args)
{
    Hash* row = map->first;

    while (row != NULL)
    {
        cb(row, args);
        row = row->next;
    }
}
