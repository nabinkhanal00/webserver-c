#include "hashmap.h"

int main() {
    Hashmap* map = hashmap_create();
    hashmap_insert(map, "hari", "shyam");
    hashmap_print(map);
    hashmap_destroy(map);
}
