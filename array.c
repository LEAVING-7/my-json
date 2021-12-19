#include "array.h"
#include <stdio.h>
#include <stdlib.h>
Array *arrayNew(size_t cap, ptr_t freeFn, ptr_t cpyFn)
{
    Array *ret;

    if ((ret = (Array *) malloc(sizeof(*ret))) == NULL)
        panic("out of memory");

    ret->values = calloc(cap, sizeof(void *));
    ret->cap = cap;
    ret->freeFn = freeFn;
    ret->cpyFn = cpyFn;
    ret->len = 0;
    return ret;
}

void arrayEmpty(Array *array)
{
    if (array->freeFn) {
        for (size_t i = 0; i < array->len; i++) {
            array->freeFn(array->values[i]);
        }
    }
}

void arrayFree(Array *array)
{
    arrayEmpty(array);
    free(array->values);
    free(array);
}

ptr_t arraySet(Array *array, size_t idx, ptr_t src)
{
    if (idx >= array->cap) {
        array->values = realloc(array->values, array->cap * 2 * sizeof(void *));
        if (!array->values) panic("realloc memory failed");
        array->cap *= 2;
    }
    void *v = array->cpyFn ? array->cpyFn(src) : src;
    array->values[idx] = v;
    array->len++;
    return NULL;
}

ptr_t arrayGet(Array *array, size_t idx)
{
    if (idx > array->len) panic("index ouf of bounds");
    return array->values[idx];
}
