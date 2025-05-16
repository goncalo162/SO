#include "cache.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

static int* create_freq_counter() {
    int* counter = malloc(sizeof(int));
    *counter = 1;
    return counter;
}

Cache* cache_create(int max_size, CachePolicy policy) {
    Cache* c = malloc(sizeof(Cache));
    c->max_size = max_size;
    c->policy = policy;
    c->table = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, freeMetadadosVoid);
    c->order = g_queue_new();
    c->freq = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, free);
    srand(time(NULL));
    return c;
}

void cache_put(Cache* c, int index, Metadados* meta) {
    int* key = malloc(sizeof(int));
    *key = index;

    int exists = g_hash_table_contains(c->table, key);
    if (exists) {
        printf("[CACHE] Substituído -> index %d (policy %d)\n", index, c->policy);
        fflush(stdout);

        // Remove da ordem se necessário
        if (c->policy == POLICY_LRU || c->policy == POLICY_FIFO)
            g_queue_remove(c->order, key);

        // Substitui entrada (liberta chave/valor anteriores)
        g_hash_table_replace(c->table, key, meta);
        g_queue_push_tail(c->order, key);

        if (c->policy == POLICY_LFU)
            g_hash_table_replace(c->freq, key, create_freq_counter());

        printf("[DEBUG][PUT] Index: %d | Total: %d | Política: %d\n",
               index, g_hash_table_size(c->table), c->policy);
        fflush(stdout);

        return;
    }

    // Se cheia, remover uma entrada
    if (g_hash_table_size(c->table) >= c->max_size) {
        gpointer remove_key = NULL;

        if (c->policy == POLICY_LRU || c->policy == POLICY_FIFO) {
            remove_key = g_queue_pop_head(c->order);
        } else if (c->policy == POLICY_LFU) {
            GList* keys = g_hash_table_get_keys(c->freq);
            int min = G_MAXINT;
            for (GList* l = keys; l != NULL; l = l->next) {
                int* val = g_hash_table_lookup(c->freq, l->data);
                if (*val < min) {
                    min = *val;
                    remove_key = l->data;
                }
            }
            g_list_free(keys);
        } else if (c->policy == POLICY_RANDOM) {
            int idx = rand() % g_queue_get_length(c->order);
            GList* elem = g_queue_peek_nth_link(c->order, idx);
            remove_key = elem ? elem->data : g_queue_peek_head(c->order);
        }

        g_hash_table_remove(c->table, remove_key);
        g_queue_remove(c->order, remove_key);
        g_hash_table_remove(c->freq, remove_key);

        printf("[CACHE] Removido -> index %d (policy %d)\n", *((int*)remove_key), c->policy);
        fflush(stdout);
    }

    g_hash_table_insert(c->table, key, meta);
    g_queue_push_tail(c->order, key);

    if (c->policy == POLICY_LFU)
        g_hash_table_insert(c->freq, key, create_freq_counter());

    printf("[CACHE] Inserido -> index %d (policy %d)\n", index, c->policy);
    fflush(stdout);
}

Metadados* cache_get(Cache* c, int index) {
    int temp = index;
    Metadados* meta = g_hash_table_lookup(c->table, &temp);

    printf("[DEBUG][GET] Index: %d | Encontrado: %s | Política: %d\n",
           index, meta ? "SIM" : "NAO", c->policy);
    fflush(stdout);

    if (meta) {
        printf("[CACHE] HIT -> index %d (policy %d)\n", index, c->policy);
        fflush(stdout);

        if (c->policy == POLICY_LRU) {
            // Remove e re-adiciona no fim (usar ponteiro real da key)
            GList* keys = c->order->head;
            while (keys) {
                int* current = keys->data;
                if (*current == index) {
                    g_queue_remove(c->order, current);
                    g_queue_push_tail(c->order, current);
                    break;
                }
                keys = keys->next;
            }
        } else if (c->policy == POLICY_LFU) {
            int* count = g_hash_table_lookup(c->freq, &temp);
            if (count) (*count)++;
        }
    }

    return meta;
}


bool cache_contains(Cache* c, int index) {
    int temp = index;
    return g_hash_table_contains(c->table, &temp);
}


void cache_free(Cache* c) {
    g_hash_table_destroy(c->table);
    g_queue_free(c->order);
    g_hash_table_destroy(c->freq);
    free(c);
}

CachePolicy parse_policy(const char* str) {
    if (g_ascii_strcasecmp(str, "LRU") == 0) return POLICY_LRU;
    if (g_ascii_strcasecmp(str, "FIFO") == 0) return POLICY_FIFO;
    if (g_ascii_strcasecmp(str, "LFU") == 0) return POLICY_LFU;
    if (g_ascii_strcasecmp(str, "RANDOM") == 0) return POLICY_RANDOM;
    return POLICY_LRU; // fallback default
}
