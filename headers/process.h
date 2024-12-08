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

typedef struct {
    char Name[100];
    float Price;
    float Score;
    int Entity;
    time_t last_modified;
    char Category[20];
    char Store[20];
} Item;

typedef struct {
    char userID[50];
    Item founded_items_in_category[100];
    int founded_items_in_category_count;
} UserSearchResults;

typedef struct {
    char userID[10];
    char itemName[20];
    float itemPrice;
    float itemScore;
    int itemEntity;
    char category[20];
    char store[20];
} Message;

typedef struct {
    Message messages[10];
    int message_count;
} ShoppingList;

typedef struct {
    pthread_t thread;
    userInfo user;
    ShoppingList shopping_list[3];
} OrderThreadArgs;

void* process_item(void* arg);

pthread_t create_thread_for_item(char item_path[], userInfo user);

void create_process_for_category(char category_path[], userInfo user);

void create_process_for_store(char store_path[], userInfo user);

void create_process_for_user(userInfo user);

#endif