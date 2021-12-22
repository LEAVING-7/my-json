#include "json.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
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

        
    JsonValue v3 = jsonFromFile("filename.json");

    JsonStack *s = jsonToString(&v3);

    jsonToFile(&v3, "haha.json");
    jsonStackFree(s);
    jsonFree(&v3);
}