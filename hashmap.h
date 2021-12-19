#ifndef HASHMAP_H
#define HASHMAP_H
#include "mytypedef.h"

typedef struct slot {
    u64 hash : 48;
    u64 dib : 16;
    union {
        long i;
        unsigned long u;
        ptr_t value;
    };
} Slot;

typedef struct hashmap {
    free_t freeFn;
    cmp_t cmpFn;
    cpy_t cpyFn;
    u64 (*hashFn)(const ptr_t);
    
    size_t cap;
    size_t len;
    size_t mask;
    Slot* slots;
} HashMap;


HashMap* hashmapNew(i32 cap,
    u64 (*hashFn)(const ptr_t),
    cmp_t cmpFn, free_t freeFn, cpy_t cpyFn);

void hashmapEmpty(HashMap* map);
void hashmapFree(HashMap *map);
ptr_t hashmapSet(HashMap* map, void* src);
ptr_t hashmapGet(HashMap* map, const ptr_t key);
uint64_t SIP64(const uint8_t* in, const size_t inlen,
    uint64_t seed0, uint64_t seed1);
#endif // HASHMAP_H