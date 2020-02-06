//
// Created by Pablo Weremczuk on 2/6/20.
//

#ifndef RAYLIBTEST_DEBUG_H
#define RAYLIBTEST_DEBUG_H

#include <stdlib.h>
#include <stdio.h>

#ifdef DEBUG_MEMORY

void* my_malloc(size_t size, char *file, int line) {
    printf("Reserving memory : %d bytes in file %s, line %d\n", size, file, line);
    return malloc(size);
}

void my_free(void *pointer, char *file, int line) {
    printf("freeng memory in position : %02x in file %s, line %d\n", pointer, file, line);
    free(pointer);
}

#define malloc(p) my_malloc(p, __FILE__, __LINE__)
#define free(p) my_free(p, __FILE__, __LINE__)

#endif

#endif //RAYLIBTEST_DEBUG_H
