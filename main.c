#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "store.h"

#include "utils/process.h"
#include "utils/file.h"

int main() {
    // char sub_dirs[100][256];
    // int store_dir_count = find_item_dirs("DatasetTest/Store1/Apparel", sub_dirs);
    
    // printf("Store count: %d\n", store_dir_count);
    // for(int i = 0; i < store_dir_count; i++) {
    //     printf("%s\n", sub_dirs[i]);
    // }


    char userID[50] = "USER1";
    // printf("ID: ");
    // scanf("%s", userID);

    create_process_for_user(userID);

    return 0;
}