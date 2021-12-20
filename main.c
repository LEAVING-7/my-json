#include "json.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
int main(int argc, char const* argv[])
{
    char const* json = "["
                       "    {"
                       "        \"name\" : \"Molecule Man\","
                       "        \"age\" : 29,"
                       "        \"secretIdentity\" : \"Dan Jukes\","
                       "        \"powers\" : ["
                       "        \"Radiation resistance\","
                       "        \"Turning tiny\","
                       "        \"Radiation blast\""
                       "        ]"
                       "    },"
                       "    {"
                       "        \"name\" : \"Madame Uppercut\","
                       "        \"age\" : 39,"
                       "        \"secretIdentity\" : \"Jane Wilson\","
                       "        \"powers\" : ["
                       "        \"Million tonne punch\","
                       "        \"Damage resistance\","
                       "        \"Superhuman reflexes\""
                       "        ]"
                       "    }"
                       "]";
    int i = strlen(json);

    JsonValue v = jsonFromFile("../test.json");
    JsonValue v2 = jsonParse(json, i);

    JsonValue get = jsonGet(&v, 3, "members", 0, "age");
    JsonValue mget = jsonGET(&v, "members", 0, "age");
    assert(get.i == mget.i);
    JsonValue powers = jsonGET(&v, "members", 2, "powers", 0);

    JsonValue age0 = jsonGET(&v2, 0, "age");
    PVL(age0.i);

    JsonValue secretIdentity1 = jsonGET(&v2, 1, "secretIdentity");
    PVL(secretIdentity1.str->v);

    jsonFree(&v2);
    jsonFree(&v);
}