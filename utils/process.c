#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <pthread.h>

#include "../headers/process.h"
#include "../headers/file.h"
#include "../headers/user.h"

#define MAX_SUB_DIRS 100
#define MAX_PATH_LEN 256

void* process_item(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    char line[100];
    // printf("PID: %d create thread for %s: %lu\n", getppid(), item_path, pthread_self());

    FILE* file = fopen(args->item_path, "r");
    if (file == NULL) {
        printf("Failed to open file\n");
        free(args);
        return NULL;
    }
    fgets(line, sizeof(line), file);

    char* token = strtok(line, " ");
    char* item_name = token;

    while (token != NULL) {
        item_name = token;
        token = strtok(NULL, " ");
    }

    size_t len = strlen(item_name);
    if (len > 0 && item_name[len - 1] == '\n') {
        item_name[len - 1] = '\0';
    }

    for (int i = 0; i < ORDER_COUNT; i++) {
        if (strcasecmp(args->user.orderList[i].name, item_name) == 0) {
            printf("found %s in %s\n", item_name, args->item_path);
            break;
        }
    }

    free(args);
    return NULL;
}

void create_thread_for_item(char item_path[], userInfo user) {
    ThreadArgs* args = (ThreadArgs*)malloc(sizeof(ThreadArgs));
    pthread_t thread;
    char* path_copy = strdup(item_path); 

    strcpy(args->item_path, item_path);
    strcpy(args->user.userID, user.userID);
    args->user.priceThreshold = user.priceThreshold;
    for (int i = 0; i < ORDER_COUNT; i++) {
        strcpy(args->user.orderList[i].name, user.orderList[i].name);
        args->user.orderList[i].count = user.orderList[i].count;
    }

    if (pthread_create(&thread, NULL, process_item, args) != 0) {
        printf("Failed to create thread");
        free(args);
        return;
    }

    pthread_join(thread, NULL);
}

void create_process_for_category(char category_path[], userInfo user) {
    pid_t pid = fork();

    if (pid == 0) {
        // printf("PID: %d create child for %s PID: %d\n", getppid(), category_path, getpid());
        char items[MAX_SUB_DIRS][MAX_PATH_LEN];
        int item_count = find_item_dirs(category_path, items);

        for (int i = 0; i < item_count; i++) create_thread_for_item(items[i], user);

        exit(0);
    }else{
        wait(NULL);
    }
}

void create_process_for_store(char store_path[], userInfo user) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // printf("PID: %d create child for %s PID: %d\n", getppid(), store_path, getpid());
        char sub_dirs[MAX_SUB_DIRS][MAX_PATH_LEN];
        int sub_dir_count = find_sub_dirs(store_path, sub_dirs);

        for (int i = 0; i < sub_dir_count; i++) create_process_for_category(sub_dirs[i], user);
        for (int i = 0; i < sub_dir_count; i++) wait(NULL);

        exit(0);
    }else{
        wait(NULL);
    }
}

void create_process_for_user(userInfo user) {
    char store_dirs[3][256];
    char sub_dirs[100][256];
    int store_dir_count = find_store_dirs(store_dirs);
    
    pid_t pid = fork();

    if (pid < 0) {
        printf("Fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // printf("%s create PID: %d\n", user.userID, getpid());

        for (int i = 0; i < store_dir_count; i++) create_process_for_store(store_dirs[i], user);
        for (int i = 0; i < store_dir_count; i++) wait(NULL);

        exit(0);
    } else {
        wait(NULL);
    }
}