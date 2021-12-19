#pragma once
#ifndef MY_TYPE_DEF_H
#define MY_TYPE_DEF_H
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

typedef void *ptr_t;


#ifndef loop
#define loop for (;;)
#endif

#ifndef AS
#define AS(vptr, type) (*(type *) (vptr))
#endif

#define ARG_COUNT(...) ARG_COUNT_IMPL(__VA_ARGS__, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)
#define ARG_COUNT_IMPL(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, COUNT, ...) COUNT


#define FMT_SELECT(expr, v)       \
    _Generic((v),                 \
             char                 \
             : #expr ": %c\n",    \
               int                \
             : #expr ": %d\n",    \
               long               \
             : #expr ": %ld\n",   \
               long long          \
             : #expr ": %lld\n",  \
               unsigned           \
             : #expr ": %u\n",    \
               unsigned long      \
             : #expr ": %lu\n",   \
               unsigned long long \
             : #expr ": %llu\n",  \
               double             \
             : #expr ": %.2f\n",  \
               float              \
             : #expr ": %.2f\n",  \
               char *             \
             : #expr ": %s\n",    \
               void *             \
             : #expr ": %p\n",    \
               default            \
             : #expr ": %d\n")



#define PVL(expr) \
    fprintf(stderr, FMT_SELECT(expr, expr), expr)

#define PVF(fmt, expr) \
    __PV(fmt, expr)

#define __PV(fmt, ...) \
    printf(#__VA_ARGS__ ": " #fmt "\n", ##__VA_ARGS__)

#define panic(_msg_, ...)                                                                 \
    do {                                                                                  \
        fprintf(stderr, "panic: " _msg_ "(%s:%d)\n", ##__VA_ARGS__, __FILE__, __LINE__); \
        exit(1);                                                                          \
    } while (0)

typedef void *(*cpy_t)(void *ptr);
typedef void (*free_t)(void *ptr);
typedef int (*cmp_t)(void *ptr, void *key);

#endif