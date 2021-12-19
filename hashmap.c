#include "hashmap.h"
#include <stdio.h>
#include <stdlib.h>


static u64 getHash(HashMap *map, const ptr_t key)
{
    return map->hashFn(key) << 16 >> 16;
}

HashMap *hashmapNew(i32 cap,
                    u64 (*hashFn)(const ptr_t),
                    cmp_t cmpFn, free_t freeFn, cpy_t cpyFn)
{
    HashMap *ret;

    if ((ret = (HashMap *) malloc(sizeof(*ret))) == NULL)
        panic("out of memory");

    i32 ncap = 16;
    if (cap < ncap) {
        cap = ncap;
    } else {
        while (ncap < cap) {
            ncap *= 2;
        }
        cap = ncap;
    }

    ret->cmpFn = cmpFn;
    ret->freeFn = freeFn;
    ret->cpyFn = cpyFn;
    ret->hashFn = hashFn;

    ret->cap = cap;
    ret->slots = (Slot *) calloc(cap, sizeof(Slot));
    ret->mask = ret->cap - 1;
    ret->len = 0;

    return ret;
}

static void freeElements(HashMap *map)
{
    if (map->freeFn) {
        for (size_t i = 0; i < map->cap; i++) {
            if (map->slots[i].dib) {
                map->freeFn(map->slots[i].value);
                map->slots[i].dib = 0;
            }
        }
    }
}

void hashmapEmpty(HashMap *map)
{
    freeElements(map);
    map->len = 0;
}

void hashmapFree(HashMap *map)
{
    freeElements(map);
    free(map->slots);
    free(map);
}

static bool resize(HashMap *map, size_t newCap)
{
    Slot *newSlots = calloc(newCap, sizeof(Slot));
    u64 newMask = newCap - 1;
    for (size_t i = 0; i < map->cap; ++i) {
        Slot entry = map->slots[i];
        if (!entry.dib) { continue; }
        entry.dib = 1;

        size_t j = entry.hash & newMask;
        loop
        {
            Slot *s = &newSlots[j];
            if (s->dib == 0) {
                *s = entry;
                break;
            }
            if (s->dib < entry.dib) {
                Slot temp = entry;
                entry = *s;
                *s = temp;
            }
            j = (j + 1) & newMask;
            entry.dib += 1;
        }
    }
    free(map->slots);
    map->slots = newSlots;
    map->mask = newMask;
    map->cap = newCap;
    return true;
}

ptr_t hashmapSet(HashMap *map, void *src)
{
    if (!src)
        panic("item is null");

    if (map->len == map->cap * 0.75) {
        if (!resize(map, map->cap * 2)) {
            panic("out of memory");
        }
    }

    Slot entry;
    if (map->cpyFn)
        entry.value = map->cpyFn(src);
    else
        entry.value = src;
    entry.hash = getHash(map, src);
    entry.dib = 1;
    size_t i = entry.hash & map->mask;
    loop
    {
        Slot *s = (map->slots + i);
        if (s->dib == 0) {
            *s = entry;
            map->len++;
            return NULL;
        }
        if (s->hash == entry.hash && map->cmpFn(entry.value, s->value) == 0) {
            // TODO 只是替换值而不是修改值
            free(s->value);
            s->value = map->cpyFn(src);
            puts("found same");
            return NULL;
            // panic("Found same item");
        }
        if (s->dib < entry.dib) {
            Slot temp = entry;
            entry = *s;
            *s = temp;
        }
        i = (i + 1) & map->mask;
        entry.dib += 1;
    }
}


ptr_t hashmapGet(HashMap *map, const ptr_t key)
{
    if (!key)
        panic("key is null");
    u64 hash = getHash(map, key);
    size_t i = hash & map->mask;
    
    loop
    {
        Slot *s = map->slots + i;
        if (!s->dib) {
            return NULL;
        }
        if (s->hash == hash && map->cmpFn(key, s->value) == 0) {
            return s->value;
        }
        i = (i + 1) & map->mask;
    }
}

uint64_t SIP64(const uint8_t *in, const size_t inlen,
               uint64_t seed0, uint64_t seed1)
{
#define U8TO64_LE(p)                                                                                                                                                                                                                             \
    {                                                                                                                                                                                                                                            \
        (((uint64_t) ((p)[0])) | ((uint64_t) ((p)[1]) << 8) | ((uint64_t) ((p)[2]) << 16) | ((uint64_t) ((p)[3]) << 24) | ((uint64_t) ((p)[4]) << 32) | ((uint64_t) ((p)[5]) << 40) | ((uint64_t) ((p)[6]) << 48) | ((uint64_t) ((p)[7]) << 56)) \
    }
#define U64TO8_LE(p, v)                             \
    {                                               \
        U32TO8_LE((p), (uint32_t) ((v)));           \
        U32TO8_LE((p) + 4, (uint32_t) ((v) >> 32)); \
    }
#define U32TO8_LE(p, v)                 \
    {                                   \
        (p)[0] = (uint8_t) ((v));       \
        (p)[1] = (uint8_t) ((v) >> 8);  \
        (p)[2] = (uint8_t) ((v) >> 16); \
        (p)[3] = (uint8_t) ((v) >> 24); \
    }
#define ROTL(x, b) (uint64_t)(((x) << (b)) | ((x) >> (64 - (b))))
#define SIPROUND           \
    {                      \
        v0 += v1;          \
        v1 = ROTL(v1, 13); \
        v1 ^= v0;          \
        v0 = ROTL(v0, 32); \
        v2 += v3;          \
        v3 = ROTL(v3, 16); \
        v3 ^= v2;          \
        v0 += v3;          \
        v3 = ROTL(v3, 21); \
        v3 ^= v0;          \
        v2 += v1;          \
        v1 = ROTL(v1, 17); \
        v1 ^= v2;          \
        v2 = ROTL(v2, 32); \
    }
    uint64_t k0 = U8TO64_LE((uint8_t *) &seed0);
    uint64_t k1 = U8TO64_LE((uint8_t *) &seed1);
    uint64_t v3 = UINT64_C(0x7465646279746573) ^ k1;
    uint64_t v2 = UINT64_C(0x6c7967656e657261) ^ k0;
    uint64_t v1 = UINT64_C(0x646f72616e646f6d) ^ k1;
    uint64_t v0 = UINT64_C(0x736f6d6570736575) ^ k0;
    const uint8_t *end = in + inlen - (inlen % sizeof(uint64_t));
    for (; in != end; in += 8) {
        uint64_t m = U8TO64_LE(in);
        v3 ^= m;
        SIPROUND;
        SIPROUND;
        v0 ^= m;
    }
    const int left = inlen & 7;
    uint64_t b = ((uint64_t) inlen) << 56;
    switch (left) {
        case 7:
            b |= ((uint64_t) in[6]) << 48;
        case 6:
            b |= ((uint64_t) in[5]) << 40;
        case 5:
            b |= ((uint64_t) in[4]) << 32;
        case 4:
            b |= ((uint64_t) in[3]) << 24;
        case 3:
            b |= ((uint64_t) in[2]) << 16;
        case 2:
            b |= ((uint64_t) in[1]) << 8;
        case 1:
            b |= ((uint64_t) in[0]);
            break;
        case 0:
            break;
    }
    v3 ^= b;
    SIPROUND;
    SIPROUND;
    v0 ^= b;
    v2 ^= 0xff;
    SIPROUND;
    SIPROUND;
    SIPROUND;
    SIPROUND;
    b = v0 ^ v1 ^ v2 ^ v3;
    uint64_t out = 0;
    U64TO8_LE((uint8_t *) &out, b);
    return out;
}