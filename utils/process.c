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
#include <mqueue.h>
#include <fcntl.h>

#define MAX_SUB_DIRS 100
#define MAX_PATH_LEN 256
#define MAX_USERS 10

typedef struct {
    char Name[100];
    float Price;
    float Score;
    int Entity;
    time_t last_modified;
    char Category[50];
} Item;

typedef struct {
    char userID[50];
    Item founded_items_in_category[100];
    int founded_items_in_category_count;
} UserSearchResults;

UserSearchResults user_search_results[MAX_USERS];
int user_search_results_count = 0;

typedef struct {
    char store_name[50];
    Item orders[100];
} ShoppingList;

int find_user_index(const char* userID, int add_user) {
    enter_critical_section(&lock);
    for (int i = 0; i < user_search_results_count; i++) {
        if (strcmp(user_search_results[i].userID, userID) == 0) {
            exit_critical_section(&lock);
            return i;
        }
    }
    int new_index = -1;
    if (add_user){
        strcpy(user_search_results[user_search_results_count].userID, userID);
        new_index = user_search_results_count++;
    }
    exit_critical_section(&lock);
    return new_index;
}

void add_item_to_category(UserSearchResults* user, const Item* item, char Category[]) {
    enter_critical_section(&lock);
    int count = user->founded_items_in_category_count;
    user->founded_items_in_category[count] = *item;
    strcpy(user->founded_items_in_category[count].Category, Category);
    user->founded_items_in_category_count++;
    exit_critical_section(&lock);
}

void* process_item(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    char line[100], log_path[MAX_PATH_LEN];
    // printf("PID: %d create thread for %s: TID:%lu\n", getppid(), args->item_path, pthread_self());

    enter_critical_section(&lock);
    sprintf(log_path, "%s/%s_OrderID.log", args->category_path, args->user.userID);
    exit_critical_section(&lock);

    FILE* log_file = fopen(log_path, "a");

    enter_critical_section(&lock);
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
    char store_name[100];
    get_store_name(args->item_path, store_name);

    char item_name[100];
    float item_price;
    float item_score;
    int item_entity;
    read_item_data(args->item_path, item_name, &item_price, &item_score, &item_entity);

    int user_index = find_user_index(args->user.userID, 1);
    UserSearchResults* user = &user_search_results[user_index];

    char category[100];
    get_category_name(args->category_path, category);

    for (int i = 0; i < ORDER_COUNT; i++) {
        if (strcasecmp(args->user.orderList[i].name, item_name) == 0) {
            Item item = {0};
            strcpy(item.Name, item_name);
            item.Price = item_price;
            item.Score = item_score;
            item.Entity = item_entity;
            item.last_modified = time(NULL);
            add_item_to_category(user, &item, category);
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

        int user_index = find_user_index(user.userID, 0);

            for (int j = 0; j < user_search_results[user_index].founded_items_in_category_count; j++) {
                printf("%d %d %d Name: %s, Price: %.2f, Score: %.2f, Entity: %d Category: %s\n", user_index, j, getpid(), 
                    user_search_results[user_index].founded_items_in_category[j].Name,
                    user_search_results[user_index].founded_items_in_category[j].Price, 
                    user_search_results[user_index].founded_items_in_category[j].Score, 
                    user_search_results[user_index].founded_items_in_category[j].Entity,
                    user_search_results[user_index].founded_items_in_category[j].Category
                );
            }

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

void* handle_orders(void *args) {
    ThreadArgs* thread_args = (ThreadArgs*)args;
    
    ShoppingList received_list;
    unsigned int priority;

    return NULL;
}

void create_thread_for_orders(userInfo user) {
    pthread_t thread;
    ThreadArgs* args = malloc(sizeof(ThreadArgs));
    
    memcpy(&args->user, &user, sizeof(userInfo));

    if (pthread_create(&thread, NULL, handle_orders, args) != 0) {
        printf("Failed to create thread");
        free(args);
        return;
    }

    pthread_detach(thread);
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

        create_thread_for_orders(user);
        for (int i = 0; i < store_dir_count; i++) create_process_for_store(store_dirs[i], user);
        for (int i = 0; i < store_dir_count; i++) wait(NULL);

        exit(0);
    } else {
        wait(NULL);
    }
}