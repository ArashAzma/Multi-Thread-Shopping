#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "store.h"

void set_data(const char* filename) {
    generate_dummy_data();
    save_data_to_binary_file(filename);

    memset(stores, 0, sizeof(stores));

    load_data_from_binary_file(filename);
}

int main() {
    const char* filename = "dataset.bin";

    set_data(filename);
    
    print_stores();

    return 0;
}