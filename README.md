# my-json

* Usage:

  ```cpp
  #include "json.h"
  #include <stdio.h>

  int main(int argc, char const* argv[])
  {
      JsonValue v = jsonFromFile("test.json");
      // function style
      JsonValue get = jsonGet(&v, 3, "members", 0, "age");
      // marco style
      JsonValue mget = jsonGET(&v, "members", 0, "age");
      jsonFree(&v);
  }
  ```
