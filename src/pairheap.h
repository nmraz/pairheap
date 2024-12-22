#pragma once

#include <stdbool.h>

struct pairheap_node {
    struct pairheap_node* parent;
    struct pairheap_node* child;
    struct pairheap_node* sibling;
};

struct pairheap {
    struct pairheap_node* root;
};

#define PAIRHEAP_INIT ((struct pairheap) {0})

typedef bool (*pairheap_is_less_func_t)(struct pairheap_node* lhs,
                                        struct pairheap_node* rhs);

static inline struct pairheap_node* pairheap_peek_min(struct pairheap* heap) {
    return heap->root;
}

void pairheap_push(struct pairheap* heap, struct pairheap_node* node,
                   pairheap_is_less_func_t is_less);

struct pairheap_node* pairheap_pop_min(struct pairheap* heap,
                                       pairheap_is_less_func_t is_less);
