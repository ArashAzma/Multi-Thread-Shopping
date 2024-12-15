#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#include "headers/process.h"
#include "headers/file.h"
#include "headers/user.h"
#include "headers/synch.h"
#include "headers/cat.h"
#include "headers/graphic.h"

void* shmem = NULL;

void* create_shared_memory(size_t size) {
  int protection = PROT_READ | PROT_WRITE;

  int visibility = MAP_SHARED | MAP_ANONYMOUS;

  return mmap(NULL, size, protection, visibility, -1, 0);
}

int main() {
    users users_list = {.user_count = 0};

    while (1) {
        shmem = create_shared_memory(sizeof(SharedThreadMessages));
        SharedThreadMessages* msg = (SharedThreadMessages*)shmem;
        strcpy(msg->messages[0].userID, "parsa1");
        strcpy(msg->messages[1].userID, "parsa2");

        printf("Main process is running\n");
        userInfo* user1 = get_user_input(&users_list);
        userInfo* user2 = get_user_input(&users_list);

        pid_t pid1 = fork();
        if (pid1 == 0) {
            create_process_for_user(user1);
            exit(0);
        }
        pid_t pid2 = fork();
        if (pid2 == 0) {
            create_process_for_user(user2);
            exit(0);
        }

        waitpid(pid1, NULL, 0);
        waitpid(pid2, NULL, 0);
        // sleep(5);

        printf("users count: %d\n", users_list.user_count);

        for (int i = 0; i < users_list.user_count; i++) {
            print_user_data(users_list.users[i]);
        }
        break;
    }

    return 0;
}