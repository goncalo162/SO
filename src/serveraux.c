#include "utils.h"
#include <glib.h>
#include <stdbool.h>

typedef struct serverAux
{
    GArray *indicesLivres;
    GHashTable *cache;
    GQueue *ordemCache;
} ServerAuxiliar;


void initStack(ServerAuxiliar *stack) 
{
    stack->indicesLivres = g_array_new(FALSE, FALSE, sizeof(int));
}

bool isStackEmpty(ServerAuxiliar *stack) {
    return stack->indicesLivres->len == 0;
}

void pushStack(ServerAuxiliar *stack, int index) {
    g_array_append_val(stack->indicesLivres, index);
}

int popStack(ServerAuxiliar *stack) {
    if (!isStackEmpty(stack)) {
        int index = g_array_index(stack->indicesLivres, int, stack->indicesLivres->len - 1);
        g_array_remove_index(stack->indicesLivres, stack->indicesLivres->len - 1);
        return index;
    }
    return -1; 
}

void freeStack(ServerAuxiliar *stack) {
    g_array_free(stack->indicesLivres, TRUE);
}