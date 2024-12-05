#ifndef PROCESS_H
#define PROCESS_H

#include "user.h"

#include <pthread.h>

#define MAX_SUB_DIRS 100
#define MAX_PATH_LEN 256

typedef struct {
    pthread_t thread;
    char category_path[MAX_PATH_LEN];
    char item_path[MAX_PATH_LEN];
    userInfo user;
} ThreadArgs;

void* process_item(void* arg);

pthread_t create_thread_for_item(char item_path[], userInfo user);

void create_process_for_category(char category_path[], userInfo user);

void create_process_for_store(char store_path[], userInfo user);

void create_process_for_user(userInfo user);

#endif