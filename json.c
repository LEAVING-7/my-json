
#include "json.h"

#define IS_VALID_TYPE(t) ((t) >= 0 && (t) <= 6)

#define IN_RANGE(ch, min, max) ((ch) >= (min) && (ch) <= (max))

#define jsonStackEMPTY(stackptr) ((stackptr)->top <= 0)
JsonStack *jsonStackNew(size_t cap)
{
    JsonStack *ret;
    cap = cap ?: 1;
    ret = (JsonStack *) malloc(sizeof(*ret));
    if (ret == NULL) panic("out of memory");

    ret->v = (char *) calloc(cap + 1, sizeof(char));
    if (ret->v == NULL) panic("out of memory");

    ret->top = 0;
    ret->cap = cap;
    return ret;
}

char *jsonStackCStr(JsonStack *stack)
{
    stack->v[stack->top] = '\0';
    return stack->v;
}

JsonStack *jsonStackFrom(char *str)
{
    size_t len = strlen(str);
    JsonStack *ret = jsonStackNew(len);
    ret->top = len;
    memcpy(jsonStackCStr(ret), str, len);
    return ret;
}


void jsonStackPush(JsonStack *stack, char v)
{
    if (stack->top >= stack->cap) {
        stack->v = (char *) realloc(stack->v, sizeof(char) * stack->cap * 2);
        if (!stack->v) panic("out of memory");
        stack->cap *= 2;
    }
    stack->v[(++stack->top) - 1] = v;
}

void jsonStackFit(JsonStack *stack)
{
    if (!stack->top) return;
    stack->v = (char *) realloc(stack->v, sizeof(char) * stack->top + 1);
    if (!stack->v) panic("out of memory");
    stack->cap = stack->top;
}

char jsonStackPop(JsonStack *stack)
{
    if (stack->top == 0) panic("stack is empty");
    else {
        char ret = stack->v[stack->top - 1];
        stack->top--;
        return ret;
    }
}


void jsonStackFree(JsonStack *stack)
{
    free(stack->v);
    free(stack);
}

// not free itself
void jsonValueFree(JsonValue *v)
{
    if (!v) return;
    switch (v->type) {
        case JSON_ARRAY:
            arrayFree(v->arr);
            break;
        case JSON_OBJECT:
            hashmapFree(v->obj);
            break;
        case JSON_STRING:
            jsonStackFree(v->str);
            break;
        default:
            break;
    }
}
// and free itself
void jsonValueFree2(JsonValue *v)
{
    if (!v) return;
    switch (v->type) {
        case JSON_ARRAY:
            arrayFree(v->arr);
            break;
        case JSON_OBJECT:
            hashmapFree(v->obj);
            break;
        case JSON_STRING:
            jsonStackFree(v->str);
            break;
        default:
            break;
    }
    free(v);
}

void *jsonValueCpy(JsonValue *v)
{
    JsonValue *ret = (JsonValue *) malloc(sizeof(*ret));
    *ret = *v;
    return ret;
}

void *_jsonPairCpy(void *p)
{
    JsonPair *ret = (JsonPair *) malloc(sizeof(*ret));
    *ret = *(JsonPair *) p;
    return ret;
}

i32 _jsonPairCmp(void *p1, void *p2)
{
    JsonPair *j1 = (JsonPair *) p1;
    JsonPair *j2 = (JsonPair *) p2;

    if (j1->k.top == j2->k.top && memcmp(j1->k.v, j2->k.v, j1->k.top) == 0)
        return 0;
    else
        return 1;
}

u64 _jsonPairHash(ptr_t in)
{
    JsonPair *p = (JsonPair *) in;
    char *str = jsonStackCStr(&p->k);
    u32 seed = 131;
    u64 hash = 0;
    while (*str) {
        hash = hash * seed + (*str++);
    }
    return hash;
}

void _jsonPairFree(void *in)
{
    JsonPair *p = (JsonPair *) in;
    free(p->k.v);
    jsonValueFree(&p->v);
    free(p);
}

void consumeWhitespace(JsonStr *s)
{
    while (isspace(s->str[s->at]))
        s->at++;
}

char getNextToken(JsonStr *s)
{
    consumeWhitespace(s);
    if (s->at == s->len) panic("unexpected end of input");
    return s->str[s->at++];
}

// TODO  not support uft-8 :(
void encodeUTF8(JsonStack *buffer, long in)
{
    if (in < 0) return;

    if (in < 0x80) {
        jsonStackPush(buffer, in);
    } else if (in < 0x800) {
        jsonStackPush(buffer, ((in >> 6) | 0xC0));
        jsonStackPush(buffer, ((in & 0x3F) | 0x80));
    } else if (in < 0x10000) {
        jsonStackPush(buffer, ((in >> 12) | 0xE0));
        jsonStackPush(buffer, (((in >> 6) & 0x3F) | 0x80));
        jsonStackPush(buffer, ((in & 0x3F) | 0x80));
    } else {
        jsonStackPush(buffer, ((in >> 18) | 0xF0));
        jsonStackPush(buffer, ((in >> 12) & 0x3F) | 0x80);
        jsonStackPush(buffer, (((in >> 6) & 0x3F) | 0x80));
        jsonStackPush(buffer, ((in & 0x3F) | 0x80));
    }
}



JsonValue parseString(JsonStr *s)
{
    JsonValue v = {
            .type = JSON_STRING,
            .str = jsonStackNew(16),
    };

    while (1) {
        if (s->at == s->len)
            panic("unexpected end of input string");
        char ch = s->str[s->at++];

        // return the string
        if (ch == '"') {
            jsonStackFit(v.str);
            return v;
        }
        // escaped character
        if (IN_RANGE(ch, 0, 0x1f))
            panic("unescaped %c in string", ch);

        // non-escaped character

        if (ch != '\\') {
            jsonStackPush(v.str, ch);
            continue;
        }

        if (s->at == s->len) panic("unexpected end of input in string");

        ch = s->str[s->at++];

        if (ch == 'u') {
            panic("sorry,not support :( ");
        }

        switch (ch) {
            case 'b':
                jsonStackPush(v.str, '\b');
                break;
            case 'f':
                jsonStackPush(v.str, '\f');
                break;
            case 'n':
                jsonStackPush(v.str, '\n');
                break;
            case 'r':
                jsonStackPush(v.str, '\r');
                break;
            case 't':
                jsonStackPush(v.str, '\t');
                break;
            case '"':
            case '\\':
            case '/':
                jsonStackPush(v.str, ch);
                break;
            default:
                panic("invalid escape character %c", ch);
        }
    }
}


#define current (s->str[s->at])

JsonValue parseNumber(JsonStr *s)
{
    JsonValue v;
    size_t startPos = s->at;

    if (s->str[s->at] == '-')
        s->at++;

    if (current == '0') {
        s->at++;
        if (IN_RANGE(current, '0', '9')) panic("leading 0 is not permitted in numbers");
    } else if (IN_RANGE(current, '1', '9')) {
        s->at++;
        while (IN_RANGE(current, '0', '9'))
            s->at++;
    } else {
        panic("invalid %c in number", current);
    }

    if (current != '.' && current != 'e' && current != 'E') {
        v.type = JSON_INT;
        v.i = atoi(s->str + startPos);
        return v;
    }

    if (current == '.') {
        s->at++;
        if (!IN_RANGE(current, '0', '9'))
            panic("at lease one digit required in fractional part");

        while (IN_RANGE(current, '0', '9'))
            s->at++;
    }

    if (current == 'e' || current == 'E') {
        s->at++;
        if (current == '+' || current == '-')
            s->at++;
        if (!IN_RANGE(current, '0', '9'))
            panic("at lease one digit required in exponent");

        while (IN_RANGE(current, '0', '9'))
            s->at++;
    }
    v.type = JSON_FLOAT;
    v.f = strtod(s->str + startPos, NULL);
    return v;
}

JsonValue expect(JsonStr *s, char *exp, bool res, JsonValueType t)
{
    JsonValue ret;
    s->at--;
    i32 len = strlen(exp);
    if (strncmp(exp, s->str + s->at, len) == 0) {
        s->at += len;
        ret.type = t;
        ret.i = res;
        return ret;
    } else {
        panic("parse bool or null error");
    }
}

JsonValue parseJson(JsonStr *s, i32 depth)
{
    if (depth > 200) {
        panic("exceed maximun nesting depth");
    }

    char ch = getNextToken(s);

    if (ch == '-' || IN_RANGE(ch, '0', '9')) {
        s->at--;
        return parseNumber(s);
    }

    if (ch == '"') return parseString(s);
    if (ch == 't') return expect(s, "true", true, JSON_BOOL);
    if (ch == 'f') return expect(s, "false", false, JSON_BOOL);
    if (ch == 'n') return expect(s, "null", NULL, JSON_NUL);


    if (ch == '{') {
        JsonValue ret;
        HashMap *map = hashmapNew(16, _jsonPairHash, _jsonPairCmp, _jsonPairFree, _jsonPairCpy);
        ch = getNextToken(s);
        if (ch == '}') {
            ret.type = JSON_OBJECT;
            ret.obj = map;
            return ret;
        }
        while (1) {
            if (ch != '"') panic("expected '\"' in object");

            JsonValue k = parseString(s);
            JsonStack key = *k.str;
            free(k.str);

            if (!IS_VALID_TYPE(k.type)) panic("invalid type at key");

            ch = getNextToken(s);
            if (ch != ':') panic("expected ':' in object");

            JsonValue v = parseJson(s, depth + 1);

            if (!IS_VALID_TYPE(v.type)) panic("invalid type at value");


            hashmapSet(map, &(JsonPair){.k = key, .v = v});

            ch = getNextToken(s);

            if (ch == '}') break;
            if (ch != ',') panic("expect ',' in object");

            ch = getNextToken(s);
        }
        ret.type = JSON_OBJECT;
        ret.obj = map;
        return ret;
    }
    if (ch == '[') {
        JsonValue v;
        Array *arr = arrayNew(4, jsonValueFree2, jsonValueCpy);
        v.arr = arr;
        v.type = JSON_ARRAY;

        ch = getNextToken(s);
        if (ch == ']') {
            return v;
        }
        while (1) {
            s->at--;
            JsonValue v = parseJson(s, depth + 1);

            arraySet(arr, arr->len, &v);

            ch = getNextToken(s);
            if (ch == ']') break;
            if (ch != ',') panic("expected ',' in list");
            ch = getNextToken(s);
            (void) ch;
        }
        return v;
    }
    panic("expected value in object");
}

// API

JsonValue jsonParse(char *str, size_t len)
{
    if (!str) panic("str is null");
    JsonStr s = {
            .str = str,
            .len = len,
            .at = 0,
    };
    JsonValue ret = parseJson(&s, 0);
    return ret;
}

void jsonFree(JsonValue *v)
{
    jsonValueFree(v);
}