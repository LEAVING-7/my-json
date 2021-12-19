#include "json.h"
#include "mytypedef.h"
#include <stdio.h>
#include <sys/stat.h>

int main(int argc, char const* argv[])
{
    char buff[79700] = {};
    FILE* test = fopen("../test.json", "r");

    struct stat s;
    stat("../test.json", &s);
    fread(buff, s.st_size, 1, test);
    PVL(s.st_size);
    PVL(buff);
    

    fclose(test);
    JsonValue v = jsonParse(buff, s.st_size);
    jsonFree(&v);
}