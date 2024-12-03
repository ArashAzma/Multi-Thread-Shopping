#ifndef PROCESS_H
#define PROCESS_H

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "file.h"
#include <pthread.h>

#define MAX_SUB_DIRS 100
#define MAX_PATH_LEN 256

void* process_item(void* arg) {
    char* item_path = (char*)arg;
    printf("PID: %d create thread for %s: %lu\n", getppid(), item_path, pthread_self());

    // read file

    free(item_path);  
    return NULL;
}

void create_thread_for_item(char item_path[]) {
    pthread_t thread;
    char* path_copy = strdup(item_path); 

    if (pthread_create(&thread, NULL, process_item, path_copy) != 0) {
        printf("Failed to create thread");
        free(path_copy);
        return;
    }

    // Detach the thread to allow independent execution
    pthread_detach(thread);  
}

void create_process_for_category(char category_path[]) {
    pid_t pid = fork();

    if (pid == 0) {
        printf("PID: %d create child for %s PID: %d\n", getppid(), category_path, getpid());
        char items[MAX_SUB_DIRS][MAX_PATH_LEN];
        int item_count = find_item_dirs(category_path, items);

        for (int i = 0; i < item_count; i++) create_thread_for_item(items[i]);

        exit(0);
    }else{
        wait(NULL);
    }
}

void create_process_for_store(char store_path[]) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        printf("PID: %d create child for %s PID: %d\n", getppid(), store_path, getpid());
        char sub_dirs[MAX_SUB_DIRS][MAX_PATH_LEN];
        int sub_dir_count = find_sub_dirs(store_path, sub_dirs);

        for (int i = 0; i < sub_dir_count; i++) create_process_for_category(sub_dirs[i]);

        exit(0);
    }else{
        wait(NULL);
    }
}

// Yani chi bar asas userID ?
void create_process_for_user(char userID[]){
    char store_dirs[3][256];
    char sub_dirs[100][256];
    int store_dir_count = find_store_dirs(store_dirs);
    int sub_dir_count = find_sub_dirs(store_dirs[1], sub_dirs);
    
    pid_t pid = fork();

    if (pid < 0) {
        printf("Fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        printf("%s create PID: %d\n", userID, getpid());

        for (int i=0; i<store_dir_count; i++) create_process_for_store(store_dirs[i]);

        exit(0);
    } else {
        wait(NULL);
    }

}

#endif 