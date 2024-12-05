#include "../headers/user.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ORDER_COUNT 3

void print_user_data(userInfo user) {
    printf("User ID: %s\n", user.userID);
    printf("Order List: \n");
    for (int i = 0; i < ORDER_COUNT; i++) printf("%s %d\n", user.orderList[i].name, user.orderList[i].count);
    printf("Price Threshold: %d\n", user.priceThreshold);
}

userInfo get_user_input() {
    char userID[50];
    order orderList[ORDER_COUNT];
    int priceThreshold;
    char priceThresholdInput[10];
    userInfo user;

    printf("Username: ");
    scanf("%s", userID);

    printf("OrderList: \n");
    for (int i = 0; i < ORDER_COUNT; i++) scanf("%s %d", orderList[i].name, &orderList[i].count);

    printf("Price threshold: \n");
    getchar();
    fgets(priceThresholdInput, sizeof(priceThresholdInput), stdin);
    if (priceThresholdInput[0] == '\n') priceThreshold = -1;
    else priceThreshold = atoi(priceThresholdInput);

    strcpy(user.userID, userID);
    for (int i = 0; i < ORDER_COUNT; i++) {
        strcpy(user.orderList[i].name, orderList[i].name);
        user.orderList[i].count = orderList[i].count;
    }
    user.priceThreshold = priceThreshold;

    return user;
}