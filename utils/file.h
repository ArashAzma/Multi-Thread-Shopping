#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SUB_DIRS 100
#define MAX_PATH_LEN 256

int find_item_dirs(char category_path[], char sub_dirs[MAX_SUB_DIRS][MAX_PATH_LEN]) {
    FILE *fp;
    char path[MAX_PATH_LEN];
    int item_count = 0;

    char command[MAX_PATH_LEN + 50];
    snprintf(command, sizeof(command), "find %s -mindepth 1 -maxdepth 1 -type f", category_path);
    fp = popen(command, "r");

    while (fgets(path, sizeof(path), fp) != NULL) {
        path[strcspn(path, "\n")] = '\0';
        strncpy(sub_dirs[item_count], path, MAX_PATH_LEN);
        item_count++;
    }

    pclose(fp);
    return item_count;
}

int find_sub_dirs(char store_path[], char sub_dirs[MAX_SUB_DIRS][MAX_PATH_LEN]) {
    FILE *fp;
    char path[MAX_PATH_LEN];
    int sub_dir_count = 0;

    char command[MAX_PATH_LEN + 50];
    snprintf(command, sizeof(command), "find %s -mindepth 1 -maxdepth 1 -type d", store_path);
    fp = popen(command, "r");

    while (fgets(path, sizeof(path), fp) != NULL) {
        path[strcspn(path, "\n")] = '\0';
        strncpy(sub_dirs[sub_dir_count], path, MAX_PATH_LEN);
        sub_dir_count++;
    }

    pclose(fp);
    return sub_dir_count;
}

int find_store_dirs(char store_dirs[][MAX_PATH_LEN]) {
    FILE *fp;
    char path[MAX_PATH_LEN];
    int store_dir_count = 0;

    // fp = popen("find Dataset -mindepth 1 -maxdepth 1 -type d", "r");
    fp = popen("find DatasetTest -mindepth 1 -maxdepth 1 -type d", "r");

    while (fgets(path, sizeof(path), fp) != NULL) {
        path[strcspn(path, "\n")] = '\0';
        strncpy(store_dirs[store_dir_count], path, MAX_PATH_LEN);
        store_dir_count++;
    }

    pclose(fp);
    return store_dir_count;
}

#endif
