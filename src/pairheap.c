#include "pairheap.h"

#include <string.h>

static void clear_node(struct pairheap_node* node) {
    memset(node, 0, sizeof(*node));
}

static void detach_subtree(struct pairheap_node* node) {
    node->parent = NULL;
    node->sibling = NULL;
}

static void add_child(struct pairheap_node* parent,
                      struct pairheap_node* child) {
    child->parent = parent;
    child->sibling = parent->child;
    parent->child = child;
}

static struct pairheap_node* merge(struct pairheap_node* a,
                                   struct pairheap_node* b,
                                   pairheap_is_less_func_t is_less) {
    if (is_less(a, b)) {
        add_child(a, b);
        return a;
    } else {
        add_child(b, a);
        return b;
    }
}

static void link_sibling(struct pairheap_node** list,
                         struct pairheap_node* new) {
    new->sibling = *list;
    *list = new;
}

static struct pairheap_node* unlink_sibling(struct pairheap_node* list) {
    struct pairheap_node* next = list->sibling;
    list->sibling = NULL;
    return next;
}

void pairheap_push(struct pairheap* heap, struct pairheap_node* node,
                   pairheap_is_less_func_t is_less) {
    // Just to be safe...
    clear_node(node);

    if (!heap->root) {
        heap->root = node;
        return;
    }

    heap->root = merge(heap->root, node, is_less);
}

struct pairheap_node* pairheap_pop_min(struct pairheap* heap,
                                       pairheap_is_less_func_t is_less) {
    if (!heap->root) {
        return NULL;
    }

    struct pairheap_node* min = heap->root;

    // First pass: merge children in pairs from newest to oldest, linking them
    // together in `merged_pairs` by temporarily hijacking `->sibling`.
    struct pairheap_node* merged_pairs = NULL;

    struct pairheap_node* pair = min->child;
    while (pair && pair->sibling) {
        struct pairheap_node* sibling = pair->sibling;
        struct pairheap_node* next_pair = sibling->sibling;

        detach_subtree(pair);
        detach_subtree(sibling);
        struct pairheap_node* merged = merge(pair, sibling, is_less);

        link_sibling(&merged_pairs, merged);
        pair = next_pair;
    }

    // Tack on the remainder if we had an odd number of children.
    if (pair) {
        detach_subtree(pair);
        link_sibling(&merged_pairs, pair);
    }

    // Second pass: merge all our merged pairs with one another in reverse
    // order, using the links temporarily set up in `->sibling`.
    struct pairheap_node* new_root = merged_pairs;
    if (new_root) {
        struct pairheap_node* next = NULL;
        struct pairheap_node* cur = unlink_sibling(new_root);
        while (cur) {
            next = unlink_sibling(cur);
            new_root = merge(new_root, cur, is_less);
            cur = next;
        }
    }

    heap->root = new_root;

    clear_node(min);
    return min;
}
