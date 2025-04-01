#include "utils.h"


typedef struct {
    GArray *indicesLivres;
} StackIndexes;


typedef struct {
    GHashTable *cache;
    GQueue *ordem;
} Cache;


void initStack(StackIndexes *stack) {
    stack->indicesLivres = g_array_new(FALSE, FALSE, sizeof(int));
}

bool isStackEmpty(StackIndexes *stack) {
    return stack->indicesLivres->len == 0;
}

void pushStack(StackIndexes *stack, int index) {
    g_array_append_val(stack->indicesLivres, index);
}

int popStack(StackIndexes *stack) {
    if (!isStackEmpty(stack)) {
        int index = g_array_index(stack->indicesLivres, int, stack->indicesLivres->len - 1);
        g_array_remove_index(stack->indicesLivres, stack->indicesLivres->len - 1);
        return index;
    }
    return -1; 
}

void freeStack(StackIndexes *stack) {
    g_array_free(stack->indicesLivres, TRUE);
}