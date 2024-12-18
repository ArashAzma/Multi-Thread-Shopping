#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <signal.h>

#include "headers/process.h"
#include "headers/file.h"
#include "headers/user.h"
#include "headers/synch.h"
#include "headers/cat.h"
#include "headers/graphic.h"

void* shmem = NULL;
void* shmem_update_score_lock = NULL;

void* create_shared_memory(size_t size) {
  return mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
}

int main() {
    users users_list = {.user_count = 0};
    printf("Main process is running\n");

    while (1) {
        int user_count_each_while = 0;
        pid_t child_pids[MAX_USERS]; 
        char command[10];
        userInfo* users_array[10];

        printf("enter user count (1 to 10): ");
        scanf("%d", &user_count_each_while);
        if (user_count_each_while < 1 || user_count_each_while > 10) continue;

        shmem_update_score_lock = create_shared_memory(sizeof(atomic_int));
        atomic_int* update_score_lock = (atomic_int*)shmem_update_score_lock;
        *update_score_lock = 0;

        shmem = create_shared_memory(sizeof(SharedThreadMessages));
        SharedThreadMessages* msg = (SharedThreadMessages*)shmem;
        msg->message_count = user_count_each_while;

        for (int i = 0; i < user_count_each_while; i++) {
            userInfo* user = get_user_input(&users_list);
            printf("NEXT\n");
            users_array[i] = user;
            strcpy(msg->messages[i].userID, user->userID);
        }

        for (int i = 0; i < user_count_each_while; i++) {
            pid_t pid = fork();
            if (pid == 0) {
                create_process_for_user(users_array[i]);
                exit(0); 
            } else if (pid > 0) {
                child_pids[i] = pid; 
            } else {
                perror("fork");
                exit(1);
            }
        }

        for (int i = 0; i < user_count_each_while; i++) wait(NULL);

        for (int i = 0; i < user_count_each_while; i++) {
            for (int j = 0; j < users_list.user_count; j++) {
                if (strcmp(users_array[i]->userID, users_list.users[j].userID) == 0) {
                    char path[50], command[50];
                    sprintf(path, "%s.txt", users_array[i]->userID);
                    FILE* file = fopen(path, "r");
                    fscanf(file, "%d %d %d", &users_list.users[j].store1_order_count, &users_list.users[j].store2_order_count, &users_list.users[j].store3_order_count);
                    fclose(file);
                    sprintf(command, "rm -rf %s", path);
                    system(command);
                }
            }
        }

        printf("users count: %d\n", users_list.user_count);

        for (int i = 0; i < users_list.user_count; i++) {
            print_user_data(users_list.users[i]);
        }
        for (int i = 0; i < user_count_each_while; i++) kill(child_pids[i], SIGKILL);
        
        printf("Do you want to continue ? (y/n): ");
        scanf("%s", command);
        if (strcmp(command, "y") != 0) break;
    }

    return 0;
}