#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "headers/process.h"
#include "headers/file.h"
#include "headers/user.h"
#include "headers/synch.h"
#include "headers/cat.h"
#include "headers/graphic.h"

int main() {
    users users_list = {.user_count = 0};

    while (1) {
        printf("Main process is running\n");
        userInfo* user = get_user_input(&users_list);
        create_process_for_user(user);

        sleep(5);

        printf("users count: %d\n", users_list.user_count);

        for (int i = 0; i < users_list.user_count; i++) {
            print_user_data(users_list.users[i]);
        }
        break;
    }

    return 0;
}