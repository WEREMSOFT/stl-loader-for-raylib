//
// Created by Pablo Weremczuk on 2/6/20.
//

#ifndef RAYLIBTEST_STACK_H
#define RAYLIBTEST_STACK_H

#define MAX_RENDEREABLES_STACK_SIZE 30

#include <raylib.h>

typedef struct rendereable_t {
    Vector3 position;
    unsigned int asset_type;
    void *asset;
} rendereable_t;

typedef struct rendereables_stack_t {
    int index;
    int max_size;
    rendereable_t rendereables[MAX_RENDEREABLES_STACK_SIZE];
} rendereables_stack_t;

int stack_push(rendereables_stack_t *stack, rendereable_t value) {
    if (stack->index < stack->max_size) {
        stack->rendereables[stack->index++] = value;
        return stack->index;
    } else {
        return -1;
    }
}

int stack_pop(rendereables_stack_t *stack, rendereable_t *value) {
    if (stack->index > 0) {
        *value = stack->rendereables[--stack->index];
        return stack->index;
    } else {
        return -1;
    }
}

bool stack_is_empty(rendereables_stack_t *stack) {
    return stack->index < 0;
}

bool stack_is_full(rendereables_stack_t *stack) {
    return stack->index == stack->max_size;
}

void stack_clean(rendereables_stack_t *stack) {
    stack->index = 0;
    stack->max_size = MAX_RENDEREABLES_STACK_SIZE;
}

#endif //RAYLIBTEST_STACK_H
