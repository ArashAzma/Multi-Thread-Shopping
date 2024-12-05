#include "../headers/process.h"
#include "../headers/file.h"
#include "../headers/user.h"
#include "../headers/synch.h"

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <pthread.h>
#include <string.h>

#define MAX_SUB_DIRS 100
#define MAX_PATH_LEN 256

void* process_item(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    char line[100], log_path[MAX_PATH_LEN];
    // printf("PID: %d create thread for %s: %lu\n", getppid(), item_path, pthread_self());

    enter_critical_section(&lock);

    sprintf(log_path, "%s/%s_OrderID.log", args->category_path, args->user.userID);
    FILE* log_file = fopen(log_path, "a");
    fprintf(log_file, "thread with ID of %lu exploring item %s\n", pthread_self(), args->item_path);
    fclose(log_file);
    
    exit_critical_section(&lock);

    FILE* file = fopen(args->item_path, "r");
    if (file == NULL) {
        printf("Failed to open file\n");
        free(args);
        return NULL;
    }
    fgets(line, sizeof(line), file);

    char item_name[100];
    strncpy(item_name, line + 6, sizeof(item_name) - 1);

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

pthread_t create_thread_for_item(char item_path[], userInfo user) {
    ThreadArgs* args = (ThreadArgs*)malloc(sizeof(ThreadArgs));
    pthread_t thread;
    char* path_copy = strdup(item_path);
    char* category_path = strdup(item_path); 

    args->thread = thread;
    strcpy(args->item_path, item_path);
    strcpy(args->user.userID, user.userID);
    args->user.priceThreshold = user.priceThreshold;
    for (int i = 0; i < ORDER_COUNT; i++) {
        strcpy(args->user.orderList[i].name, user.orderList[i].name);
        args->user.orderList[i].count = user.orderList[i].count;
    }
    char* last_slash = strrchr(category_path, '/');
    *last_slash = '\0';
    strcpy(args->category_path, category_path);

    if (pthread_create(&thread, NULL, process_item, args) != 0) {
        printf("Failed to create thread");
        free(args);
        return;
    }

    return thread;
}

void create_process_for_category(char category_path[], userInfo user) {
    pid_t pid = fork();

    if (pid == 0) {
        // printf("PID: %d create child for %s PID: %d\n", getppid(), category_path, getpid());

        char log_path[MAX_PATH_LEN];
        sprintf(log_path, "%s/%s_OrderID.log", category_path, user.userID);
        FILE* log_file = fopen(log_path, "w");
        fprintf(log_file, "PID of the process exploring in this category: %d\n", getpid());
        fclose(log_file);

        char items[MAX_SUB_DIRS][MAX_PATH_LEN];
        int item_count = find_item_dirs(category_path, items);

        pthread_t threads[item_count];
        for (int i = 0; i < item_count; i++) threads[i] = create_thread_for_item(items[i], user);
        for (int i = 0; i < item_count; i++) pthread_join(threads[i], NULL);

        exit(0);
    } else {
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
    } else {
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