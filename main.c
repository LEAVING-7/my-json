#include "json.h"
#include <stdio.h>

int main(int argc, char const* argv[])
{
    JsonValue v = jsonFromFile("../test.json");
    JsonValue get = jsonGet(&v, 3, "members", 0, "age");
    JsonValue mget = jsonGET(&v, "members", 0, "age");

    PVL(get.type);
    PVL(get.i);

    jsonFree(&v);
}