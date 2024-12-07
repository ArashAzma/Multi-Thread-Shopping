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
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 256
#define QUEUE_PERMISSIONS 0660

typedef struct {
    char store_name[50];
    char item_name[100];
    int quantity;
    float price;
} OrderMessage;

mqd_t order_queue;

void setup_message_queue() {
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = sizeof(OrderMessage);
    attr.mq_curmsgs = 0;

    order_queue = mq_open("/order_processing_queue", 
                          O_CREAT | O_RDWR, 
                          QUEUE_PERMISSIONS, 
                          &attr);
    
    if (order_queue == (mqd_t)-1) {
        perror("mq_open failed");
        exit(EXIT_FAILURE);
    }
}

void send_order_to_queue(const char* store_name, const char* item_name, int quantity, float price) {
    OrderMessage msg;
    
    strncpy(msg.store_name, store_name, sizeof(msg.store_name) - 1);
    strncpy(msg.item_name, item_name, sizeof(msg.item_name) - 1);
    msg.quantity = quantity;
    msg.price = price;

    if (mq_send(order_queue, (char*)&msg, sizeof(OrderMessage), 0) == -1) perror("mq_send failed");
}

// Cleanup function (call this when you're done)
void cleanup_message_queue() {
    mq_close(order_queue);
    mq_unlink("/order_processing_queue");
}

void* process_item(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    char line[100], log_path[MAX_PATH_LEN];
    // printf("PID: %d create thread for %s: TID:%lu\n", getppid(), args->item_path, pthread_self());

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
    float item_price;
    float item_score;
    int item_entity;
    read_item_data(args->item_path, item_name, &item_price, &item_score, &item_entity);
    for (int i = 0; i < ORDER_COUNT; i++) {
        if (strcasecmp(args->user.orderList[i].name, item_name) == 0) {

            printf("found %s in %s\n", item_name, args->item_path);
            send_order_to_queue(args->item_path, args->user.orderList[i].name, args->user.orderList[i].count, item_price);
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

void* handle_orders(void *args) {
    ThreadArgs* thread_args = (ThreadArgs*)args;
    
    // printf("PID: %d create thread for Orders TID: %lu\n", getppid(), pthread_self());

    OrderMessage received_msg;
    unsigned int priority;

    while (1) {
        ssize_t bytes_read = mq_receive(order_queue, 
                                        (char*)&received_msg, 
                                        sizeof(OrderMessage), 
                                        &priority);
        
        if (bytes_read == -1) {
            perror("mq_receive failed");
            break;
        }

        printf("Received order: Item %s, Quantity %d, from store %s, Price %f\n", 
               received_msg.item_name, 
               received_msg.quantity, 
               received_msg.store_name,
               received_msg.price
               );

    }

    return NULL;
}

void create_thread_for_orders(userInfo user) {
    setup_message_queue();

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