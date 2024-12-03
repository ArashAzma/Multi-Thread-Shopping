#ifndef PROCESS_H
#define PROCESS_H

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "file.h"

void create_process_for_category(char category_path[]) {
    pid_t pid = fork();

    if (pid == 0) {
        printf("PID: %d create child for %s PID: %d\n", getppid(), category_path, getpid());

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

        exit(EXIT_SUCCESS);
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