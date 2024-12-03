#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SUB_DIRS 100
#define MAX_PATH_LEN 256

int find_sub_dirs(char sub_dirs[MAX_SUB_DIRS][MAX_PATH_LEN]) {
    FILE *fp;
    char path[MAX_PATH_LEN];
    int sub_dir_count = 0;

    fp = popen("find Dataset -mindepth 2 -maxdepth 2 -type d", "r");

    while (fgets(path, sizeof(path), fp) != NULL) {
        path[strcspn(path, "\n")] = '\0';
        strncpy(sub_dirs[sub_dir_count], path, MAX_PATH_LEN);
        sub_dir_count++;
    }

    pclose(fp);
    return sub_dir_count;
}

#endif
