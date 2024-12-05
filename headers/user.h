#ifndef USER_H
#define USER_H

#define ORDER_COUNT 3

typedef struct Order {
    char name[50];
    int count;
} order;

typedef struct UserInfo {
    char userID[50];
    order orderList[ORDER_COUNT];
    int priceThreshold;
    
} userInfo;

void print_user_data(userInfo user) {}

userInfo get_user_input() {}

#endif