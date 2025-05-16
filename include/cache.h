#ifndef CACHE_H
#define CACHE_H

#include <glib.h>
#include "metadados.h"

typedef enum {
    POLICY_LRU,
    POLICY_FIFO,
    POLICY_LFU,
    POLICY_RANDOM
} CachePolicy;

typedef struct {
    int max_size;
    CachePolicy policy;
    GHashTable* table; // key: GINT_TO_POINTER(index), value: Metadados*
    GQueue* order;     // LRU/FIFO/RANDOM tracking
    GHashTable* freq;  // LFU: key=index, value=int*
} Cache;

Cache* cache_create(int max_size, CachePolicy policy);
void cache_put(Cache* c, int index, Metadados* meta);
Metadados* cache_get(Cache* c, int index);
bool cache_contains(Cache* c, int index);
void cache_free(Cache* c);
CachePolicy parse_policy(const char* str);

#endif // CACHE_H
