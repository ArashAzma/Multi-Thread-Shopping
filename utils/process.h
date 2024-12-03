#ifndef PROCESS_H
#define PROCESS_H

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "file.h"

// Yani chi bar asas userID ?
void create_process_for_user(char userID[]){
    char sub_dirs[100][256];
    int sub_dir_count = find_sub_dirs(sub_dirs);

    pid_t pid = fork();

    if (pid < 0) {
        printf("Fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        printf("Child process created for user: %s\n", userID);


        exit(0);
    } else {
        printf("Parent process, child PID: %d\n", pid);
        wait(NULL);
    }

}

#endif 