#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "store.h"

#include "utils/process.h"
#include "utils/file.h"

int main() {
    char userID[50];
    // printf("ID: ");
    // scanf("%s", userID);

    create_process_for_user(userID);

    return 0;
}