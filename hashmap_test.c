// #include <stdio.h>
// #include <stdlib.h>

#include "hashmap.h"

int main() {
    hashmap* map = hashmap_create();
    hashmap_print(map);
    hashmap_destroy(map);
}
