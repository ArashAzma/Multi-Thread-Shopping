#include "../headers/file.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SUB_DIRS 100
#define MAX_PATH_LEN 256

void get_category_name(char path[], char category_name[]) {
    // DatasetTest/Store1/Digital
    char path_copy[MAX_PATH_LEN];
    strncpy(path_copy, path, MAX_PATH_LEN);
    path_copy[MAX_PATH_LEN - 1] = '\0';

    char* last_slash = strrchr(path_copy, '/');
    
    if (last_slash != NULL) {
        strcpy(category_name, last_slash + 1);
    } else {
        strcpy(category_name, path_copy);
    }
}

void get_store_name(char path[], char store_name[]) {
    // DatasetTest/Store3/Beauty
    char path_copy[MAX_PATH_LEN];
    strncpy(path_copy, path, MAX_PATH_LEN);
    path_copy[MAX_PATH_LEN - 1] = '\0';

    char *token = strtok(path_copy, "/");
    while (token != NULL) {
        if (strncmp(token, "Store", 5) == 0) {
            strcpy(store_name, token);
            return;
        }
        token = strtok(NULL, "/");
    }
    strcpy(store_name, "Unknown");
}


void read_item_data(char item_path[], char item_name[], float *item_price, float *item_score, int *item_entity) {
    FILE* item_file = fopen(item_path, "r");
    if (item_file == NULL) {
        perror("Failed to open file");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), item_file)) {
        if (strncmp(line, "Name:", 5) == 0) {
            sscanf(line, "Name: %255s", item_name);
        } else if (strncmp(line, "Price:", 6) == 0) {
            sscanf(line, "Price: %f", item_price);
        } else if (strncmp(line, "Score:", 6) == 0) {
            sscanf(line, "Score: %f", item_score);
        } else if (strncmp(line, "Entity:", 7) == 0) {
            sscanf(line, "Entity: %d", item_entity);
        }
    }

    fclose(item_file);
}

int find_item_dirs(char category_path[], char sub_dirs[MAX_SUB_DIRS][MAX_PATH_LEN]) {
    FILE *fp;
    char path[MAX_PATH_LEN];
    int item_count = 0;

    char command[MAX_PATH_LEN + 50];
    snprintf(command, sizeof(command), "find %s -mindepth 1 -maxdepth 1 -type f ! -name '*.log'", category_path);
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