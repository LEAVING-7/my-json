#include "array.h"
#include "hashmap.h"
#include "mytypedef.h"


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum JsonValueType {
    JSON_NUL,
    JSON_FLOAT,
    JSON_INT,
    JSON_BOOL,
    JSON_STRING,
    JSON_ARRAY,
    JSON_OBJECT,
} JsonValueType;

typedef struct JsonStack JsonStack;

typedef struct JsonValue {
    JsonValueType type;
    union {
        bool b;
        f64 f;
        i64 i;
        Array *arr;
        HashMap *obj;
        JsonStack *str;
    };
} JsonValue;

typedef struct {
    char *str;
    size_t len;
    size_t at;
} JsonStr;

typedef struct JsonStack {
    size_t cap, top;
    char *v;
} JsonStack;

typedef struct {
    JsonStack k;
    JsonValue v;
} JsonPair;
// API
JsonValue jsonParse(char *str, size_t len);
void jsonFree(JsonValue *v);