#ifndef USER_H
#define USER_H

#define ORDER_COUNT 3
#define USER_ID_LENGTH 50
#define PRICE_THRESHOLD_INPUT 10

typedef struct Order {
    char name[50];
    int count;
} order;

typedef struct UserInfo {
    char userID[50];
    order orderList[ORDER_COUNT];
    int priceThreshold;
    int store1_order_count;
    int store2_order_count;
    int store3_order_count;
} userInfo;

typedef struct Users {
    userInfo users[10];
    int user_count;
} users;

void print_user_data(userInfo user);

userInfo* get_user_input(users* users_list);

#endif