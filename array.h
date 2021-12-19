#pragma once
#include "mytypedef.h"

typedef struct array {
    free_t freeFn;
    cpy_t cpyFn;
    size_t len;
    size_t cap; 
    void **values;
} Array;

#define arrayAT(arrayptr, idx) ((*(arrayptr)->values)[(idx)])

Array *arrayNew(size_t cap, ptr_t freeFn, ptr_t cpyFn);
ptr_t arrayGet(Array *array, size_t idx);
ptr_t arraySet(Array *array, size_t idx, ptr_t src);
void arrayFree(Array *array);
void arrayEmpty(Array *array);

