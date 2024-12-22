#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pairheap.h"

#define ASSERT(c)                                                              \
    do {                                                                       \
        if (!(c))                                                              \
            abort();                                                           \
    } while (0)

#define DEBUG 0

#if DEBUG
#define TRACE(...) printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

struct test_node {
    struct pairheap_node node;
    int value;
};

static bool test_node_less(struct pairheap_node* lhs,
                           struct pairheap_node* rhs) {
    return ((struct test_node*) lhs)->value < ((struct test_node*) rhs)->value;
}

static struct pairheap_node* make_node(int value) {
    struct test_node* node = calloc(1, sizeof(*node));
    node->value = value;
    return (struct pairheap_node*) node;
}

static void verify_node(struct pairheap_node* node) {
    for (struct pairheap_node* child = node->child; child;
         child = child->sibling) {
        ASSERT(((struct test_node*) node)->value <=
               ((struct test_node*) child)->value);
        verify_node(child);
    }
}

static void verify_heap(struct pairheap* heap) {
    if (heap->root) {
        verify_node(heap->root);
    }
}

static void dump_node(struct pairheap_node* node, int indent) {
    for (int i = 0; i < indent; i++) {
        TRACE("  ");
    }

    TRACE("- %d\n", ((struct test_node*) node)->value);
    for (struct pairheap_node* child = node->child; child;
         child = child->sibling) {
        dump_node(child, indent + 1);
    }
}

static void dump_heap(struct pairheap* heap) {
    if (heap->root) {
        dump_node(heap->root, 0);
    }
}

static void destroy_heap(struct pairheap* heap) {
    struct pairheap_node* node = pairheap_pop_min(heap, test_node_less);
    while (node) {
        TRACE("DESTROY %d\n", ((struct test_node*) node)->value);
        dump_heap(heap);
        free(node);
        node = pairheap_pop_min(heap, test_node_less);
    }
}

enum heap_op_kind : uint8_t {
    PUSH,
    POP_MIN,
};

struct __attribute__((packed)) heap_op {
    enum heap_op_kind kind;
    int value;
};

int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    struct pairheap heap = PAIRHEAP_INIT;

    size_t count = size / sizeof(struct heap_op);
    for (size_t i = 0; i < count; i++) {
        struct heap_op op;
        memcpy(&op, data + i * sizeof(struct heap_op), sizeof(struct heap_op));

        switch (op.kind) {
        case PUSH:
            TRACE("PUSH %d\n", op.value);
            pairheap_push(&heap, make_node(op.value), test_node_less);
            verify_heap(&heap);
            dump_heap(&heap);
            break;
        case POP_MIN:
            struct pairheap_node* node =
                pairheap_pop_min(&heap, test_node_less);
            TRACE("POP %d\n", op.value);
            verify_heap(&heap);
            dump_heap(&heap);
            free(node);
            break;
        default:
            break;
        }
    }

    destroy_heap(&heap);

    return 0;
}
