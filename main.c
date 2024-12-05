#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "headers/process.h"
#include "headers/file.h"
#include "headers/user.h"
#include "headers/synch.h"

int main() {
    while (1) {
        printf("Main process is running\n");
        userInfo user = get_user_input();
        create_process_for_user(user);
        sleep(10);
    }

    return 0;
}