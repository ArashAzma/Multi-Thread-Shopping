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
#include <sys/ipc.h>
#include <fcntl.h>

#define MAX_SUB_DIRS 100
#define MAX_PATH_LEN 256
#define MAX_USERS 10

#define MQ_MAX_MESSAGES 10
#define MQ_MAX_MSG_SIZE sizeof(UserSearchResults)

#define QUEUE_NAME "/store_category_queue"
#define MAX_MSG_SIZE 1024
#define MAX_MESSAGES 10

#define MAX_STORES 3

UserSearchResults user_search_results[MAX_USERS];
int user_search_results_count = 0;

mqd_t init_message_queue() {
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = sizeof(Message);
    attr.mq_curmsgs = 0;

    mqd_t mq = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, 0644, &attr);
    if (mq == (mqd_t)-1) {
        perror("Failed to create message queue");
        exit(EXIT_FAILURE);
    }
    return mq;
}

void send_message(mqd_t mq, UserSearchResults* user, int user_index) {
    for (int j = 0; j < user->founded_items_in_category_count; j++) {
        Message msg;
        strcpy(msg.userID, user->userID);
        strcpy(msg.itemName, user->founded_items_in_category[j].Name);
        msg.itemPrice = user->founded_items_in_category[j].Price;
        msg.itemScore = user->founded_items_in_category[j].Score;
        msg.itemEntity = user->founded_items_in_category[j].Entity;
        strcpy(msg.category, user->founded_items_in_category[j].Category);
        strcpy(msg.store, user->founded_items_in_category[j].Store);
        msg.itemValue = user->founded_items_in_category[j].value;

        if (mq_send(mq, (const char*)&msg, sizeof(Message), 0) == -1) {
            perror("Failed to send message");
        }
    }
}

void receive_messages(mqd_t mq, ShoppingList shopping_list[]) {
    Message msg;
    shopping_list[0].message_count = 0;
    shopping_list[1].message_count = 0;
    shopping_list[2].message_count = 0;

    shopping_list[0].total_value = 0;
    shopping_list[1].total_value = 0;
    shopping_list[2].total_value = 0;

    shopping_list[0].total_price = 0;
    shopping_list[1].total_price = 0;
    shopping_list[2].total_price = 0;

    while (1) {
        if (mq_receive(mq, (char*)&msg, sizeof(Message), NULL) == -1) break;

        int index = -1;
        if(strcmp(msg.store, "Store1") == 0) {
            index = 0;
        } else if(strcmp(msg.store, "Store2") == 0) {
            index = 1;
        } else if(strcmp(msg.store, "Store3") == 0) {
            index = 2;
        }
        enter_critical_section(&shopping_lock);
        shopping_list[index].messages[shopping_list[index].message_count] = msg;
        shopping_list[index].message_count++;
        exit_critical_section(&shopping_lock);
        // printf("User: %s, Store: %s, Item: %s, Price: %.2f, Score: %.2f, Entity: %d, Category: %s\n",
        //     msg.userID, msg.store, msg.itemName, msg.itemPrice, msg.itemScore, msg.itemEntity, msg.category);
    }

    return shopping_list;
}

int find_user_index(const char* userID, int add_user) {
    enter_critical_section(&user_lock);
    for (int i = 0; i < user_search_results_count; i++) {
        if (strcmp(user_search_results[i].userID, userID) == 0) {
            exit_critical_section(&user_lock);
            return i;
        }
    }
    int new_index = -1;
    if (add_user){
        strcpy(user_search_results[user_search_results_count].userID, userID);
        new_index = user_search_results_count++;
    }
    exit_critical_section(&user_lock);
    return new_index;
}

void add_item_to_category(UserSearchResults* user, const Item* item, char Category[]) {
    enter_critical_section(&category_lock);
    int count = user->founded_items_in_category_count;
    user->founded_items_in_category[count] = *item;
    strcpy(user->founded_items_in_category[count].Category, Category);
    user->founded_items_in_category_count++;
    exit_critical_section(&category_lock);
}

void* process_item(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;

    char line[100], log_path[MAX_PATH_LEN];
    // printf("PID: %d create thread for %s: TID:%lu\n", getppid(), args->item_path, pthread_self());

    enter_critical_section(&file_lock);
    sprintf(log_path, "%s/%s_OrderID.log", args->category_path, args->user->userID);
    FILE* log_file = fopen(log_path, "a");
    fprintf(log_file, "thread with ID of %lu exploring item %s\n", pthread_self(), args->item_path);
    fclose(log_file);
    exit_critical_section(&file_lock);

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

    int user_index = find_user_index(args->user->userID, 1);
    UserSearchResults* user = &user_search_results[user_index];

    char category[100];
    get_category_name(args->category_path, category);

    for (int i = 0; i < ORDER_COUNT; i++) {
        if (strcasecmp(args->user->orderList[i].name, item_name) == 0 && args->user->orderList[i].count <= item_entity) {
            Item item = {0};
            strcpy(item.Name, item_name);
            item.Price = item_price;
            item.Score = item_score;
            item.Entity = item_entity;
            item.last_modified = time(NULL);
            item.value = item_price * item_score;
            strcpy(item.Store, store_name);

            printf("User: %s, Item: %s, Price: %.2f, Score: %.2f, Entity: %d, Category: %s\n",
                args->user->userID, item_name, item_price, item_score, item_entity, category);

            add_item_to_category(user, &item, category);
            break;
        }
    }
    free(args);

    return NULL;
}

void* thread_job(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    // // printf("%lu PROCESSING : %s\n", pthread_self(), args->item_path);
    process_item(args);
    // while(1){
    //     if(!args->sw){
    //         // printf("%lu PROCESSING : %s\n", pthread_self(), args->item_path);
    //         process_item(args);
    //         args->sw=1;
    //     }
    //     // printf("ALIVE : %lu\n", pthread_self());
    //     sleep(3);
    // }
}

pthread_t create_thread_for_item(char item_path[], userInfo* user) {
    ThreadArgs* args = (ThreadArgs*)malloc(sizeof(ThreadArgs));
    pthread_t thread;
    char* path_copy = strdup(item_path);
    char* category_path = strdup(item_path); 

    args->thread = thread;
    strcpy(args->item_path, item_path);
    args->user = user;
    args->sw = 0;
    // strcpy(args->user->userID, user->userID);
    // args->user->priceThreshold = user->priceThreshold;
    // for (int i = 0; i < ORDER_COUNT; i++) {
    //     strcpy(args->user->orderList[i].name, user->orderList[i].name);
    //     args->user->orderList[i].count = user->orderList[i].count;
    // }
    char* last_slash = strrchr(category_path, '/');
    *last_slash = '\0';
    strcpy(args->category_path, category_path);

    if (pthread_create(&thread, NULL, thread_job, args) != 0) {
        printf("Failed to create thread");
        free(args);
        return;
    }

    return thread;
}

void create_process_for_category(char category_path[], userInfo* user) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // printf("PID: %d create child for %s PID: %d\n", getppid(), category_path, getpid());

        char log_path[MAX_PATH_LEN];
        sprintf(log_path, "%s/%s_OrderID.log", category_path, user->userID);
        FILE* log_file = fopen(log_path, "w");
        fprintf(log_file, "PID of the process exploring in this category: %d\n", getpid());
        fclose(log_file);

        char items[MAX_SUB_DIRS][MAX_PATH_LEN];
        int item_count = find_item_dirs(category_path, items);

        pthread_t threads[item_count];
        for (int i = 0; i < item_count; i++) threads[i] = create_thread_for_item(items[i], user);
        // for (int i = 0; i < item_count; i++) pthread_join(threads[i], NULL);
        for (int i = 0; i < item_count; i++) pthread_detach(threads[i]);
        for (long int i=0; i<5000000000 ;i++);

        int user_index = find_user_index(user->userID, 0);
        mqd_t mq = mq_open(QUEUE_NAME, O_WRONLY);
        if (mq == (mqd_t)-1) {
            perror("Failed to open message queue in category process");
            exit(EXIT_FAILURE);
        }
        printf("KIR %d \n", user_search_results[user_index].founded_items_in_category_count);
        for (int i = 0; i < user_search_results[user_index].founded_items_in_category_count; i++) {
            printf("KIR User: %s, Item: %s, Price: %.2f, Score: %.2f, Entity: %d, Category: %s\n",
                user_search_results[user_index].userID, user_search_results[user_index].founded_items_in_category[i].Name,
                user_search_results[user_index].founded_items_in_category[i].Price, user_search_results[user_index].founded_items_in_category[i].Score,
                user_search_results[user_index].founded_items_in_category[i].Entity, user_search_results[user_index].founded_items_in_category[i].Category);
        
        }

        send_message(mq, &user_search_results[user_index], user_index);
        exit(0);
        mq_close(mq);
    } else {
        wait(NULL);
    }
}

void create_process_for_store(char store_path[], userInfo* user) {
    pid_t pids[MAX_SUB_DIRS];
    char sub_dirs[MAX_SUB_DIRS][MAX_PATH_LEN];
    int sub_dir_count = find_sub_dirs(store_path, sub_dirs);

    for (int i = 0; i < sub_dir_count; i++) {
        pids[i] = fork();
        if (pids[i] == 0) {
            create_process_for_category(sub_dirs[i], user);
            // printf("PID: %d create child for %s PID: %d\n", getppid(), store_path, getpid());
            exit(0); 
        }
    }

    for (int i = 0; i < sub_dir_count; i++) waitpid(pids[i], NULL, 0); 
}

void* handle_orders(void *args) {
    enter_critical_section(&msq_lock);
    OrderThreadArgs* order_args = (OrderThreadArgs*)args;
    // while (1) {
    //     enter_critical_section(order_args->lock);
    //     if (order_args->shopping_list != NULL && strcmp(order_args->shopping_list, "") != 0) {
    //         printf("Shopping list is ready\n");
    //         exit_critical_section(order_args->lock);
    //         break;
    //     }
    //     exit_critical_section(order_args->lock);
    // }

    for (int i = 0; i < MAX_STORES; i++) {
        printf("Store%d %d\n", i + 1, order_args->shopping_list[i].message_count);
        for (int j = 0; j < order_args->shopping_list[i].message_count; j++) {
            printf("User: %s, Item: %s, Price: %.2f, Score: %.2f, Entity: %d, Category: %s\n",
                order_args->shopping_list[i].messages[j].userID, order_args->shopping_list[i].messages[j].itemName, order_args->shopping_list[i].messages[j].itemPrice,
                order_args->shopping_list[i].messages[j].itemScore, order_args->shopping_list[i].messages[j].itemEntity, order_args->shopping_list[i].messages[j].category);
            
            for (int k = 0; k < ORDER_COUNT; k++) {
                if (strcasecmp(order_args->shopping_list[i].messages[j].itemName, order_args->user->orderList[k].name) == 0) {
                    order_args->shopping_list[i].total_price += order_args->shopping_list[i].messages[j].itemPrice * order_args->user->orderList[k].count;
                    order_args->shopping_list[i].total_value += order_args->shopping_list[i].messages[j].itemValue;
                    break;
                }
            }
        }
        printf("Total price: %.2f, Total value: %.2f\n", order_args->shopping_list[i].total_price, order_args->shopping_list[i].total_value);

        if (order_args->best_shopping_list_indexes[0] == -1) order_args->best_shopping_list_indexes[i] = i;
        else if (order_args->shopping_list[i].total_value > order_args->shopping_list[order_args->best_shopping_list_indexes[0]].total_value) {
            order_args->best_shopping_list_indexes[2] = order_args->best_shopping_list_indexes[1];
            order_args->best_shopping_list_indexes[1] = order_args->best_shopping_list_indexes[0];
            order_args->best_shopping_list_indexes[0] = i;
        } else if (order_args->shopping_list[i].total_value > order_args->shopping_list[order_args->best_shopping_list_indexes[1]].total_value) {
            order_args->best_shopping_list_indexes[2] = order_args->best_shopping_list_indexes[1];
            order_args->best_shopping_list_indexes[1] = i;
        } else {
            order_args->best_shopping_list_indexes[2] = i;
        }
    }

    printf("Best shopping lists in order based on total_value: %d %d %d\n", order_args->best_shopping_list_indexes[0], order_args->best_shopping_list_indexes[1], order_args->best_shopping_list_indexes[2]);

    // free(order_args);
    exit_critical_section(&order_lock);
    exit_critical_section(&msq_lock);
    return NULL;
}

void* handle_scores(void *args) {
}

void* handle_final(void *args) {
    enter_critical_section(&order_lock);
    OrderThreadArgs* order_args = (OrderThreadArgs*)args;
    if (order_args->user->priceThreshold >= order_args->shopping_list[order_args->best_shopping_list_indexes[0]].total_price) {
        printf("Best order for user %s is finalized\n", order_args->user->userID);
        order_args->user->order_count++;
    } else if (order_args->user->priceThreshold >= order_args->shopping_list[order_args->best_shopping_list_indexes[1]].total_price) {
        printf("Second best order for user %s is finalized\n", order_args->user->userID);
        order_args->user->order_count++;
    } else if (order_args->user->priceThreshold >= order_args->shopping_list[order_args->best_shopping_list_indexes[2]].total_price) {
        printf("Third best order for user %s is finalized\n", order_args->user->userID);
        order_args->user->order_count++;
    } else {
        printf("No order for user %s is finalized\n", order_args->user->userID);
    }
    exit_critical_section(&order_lock);
}

pthread_t create_thread_for_orders(userInfo* user, OrderThreadArgs* args) {
    pthread_t thread;
    
    args->user = user;

    if (pthread_create(&thread, NULL, handle_orders, args) != 0) {
        perror("Failed to create thread");
        free(args);
        exit(EXIT_FAILURE);
    }
    return thread;
}

pthread_t create_thread_for_scores(userInfo* user, OrderThreadArgs* args) {
    pthread_t thread;
    
    args->user = user;

    if (pthread_create(&thread, NULL, handle_scores, args) != 0) {
        perror("Failed to create thread");
        free(args);
        exit(EXIT_FAILURE);
    }
    return thread;
}

pthread_t create_thread_for_final(userInfo* user, OrderThreadArgs* args) {
    pthread_t thread;
    
    args->user = user;

    if (pthread_create(&thread, NULL, handle_final, args) != 0) {
        perror("Failed to create thread");
        free(args);
        exit(EXIT_FAILURE);
    }
    return thread;
}

void create_process_for_user(userInfo* user) {
    char store_dirs[3][256];
    int store_dir_count = find_store_dirs(store_dirs);

    mq_unlink(QUEUE_NAME);
    init_message_queue();

    pid_t store_pids[store_dir_count];

    for (int i = 0; i < store_dir_count; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            create_process_for_store(store_dirs[i], user);
            // printf("%s create PID: %d\n", user->userID, getpid());
            exit(0); 
        } else {
            store_pids[i] = pid;
        }
    }

    for (int i = 0; i < store_dir_count; i++) waitpid(store_pids[i], NULL, 0);

    OrderThreadArgs* args = malloc(sizeof(OrderThreadArgs));
    args->best_shopping_list_indexes[0] = -1;
    args->best_shopping_list_indexes[1] = -1;
    args->best_shopping_list_indexes[2] = -1;

    sleep(3);
    
    mqd_t mq = mq_open(QUEUE_NAME, O_RDONLY | O_NONBLOCK);
    if (mq == (mqd_t)-1) {
        perror("Failed to open message queue in user process");
        exit(EXIT_FAILURE);
    }

    ShoppingList shopping_list[MAX_STORES];
    receive_messages(mq, shopping_list);

    enter_critical_section(&shopping_lock);
    memcpy(args->shopping_list, shopping_list, sizeof(shopping_list));
    exit_critical_section(&shopping_lock);

    pthread_t orderThread = create_thread_for_orders(user, args);
    pthread_t scoreThread = create_thread_for_scores(user, args);
    pthread_t finalThread = create_thread_for_final(user, args);

    mq_close(mq);

    pthread_join(orderThread, NULL);
    pthread_join(scoreThread, NULL);
    pthread_join(finalThread, NULL);

    free(args);
}
