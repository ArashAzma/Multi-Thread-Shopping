#include "../headers/user.h"
#include "../headers/graphic.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_user_data(userInfo user) {
    printf("\n\n\n----------------------------------------------\n");
    printf("User ID: %s\n", user.userID);
    printf("Order List: \n");
    for (int i = 0; i < ORDER_COUNT; i++) printf("%s %d\n", user.orderList[i].name, user.orderList[i].count);
    printf("Price Threshold: %d\n", user.priceThreshold);
    printf("Store1 Order Count: %d\n", user.store1_order_count);
    printf("Store2 Order Count: %d\n", user.store2_order_count);
    printf("Store3 Order Count: %d\n", user.store3_order_count);
    printf("----------------------------------------------\n\n\n");
}

userInfo* get_user_input(users* users_list) {
    int user_founded = 0;
    char userID[USER_ID_LENGTH];
    order orderList[ORDER_COUNT];
    int priceThreshold;
    char priceThresholdInput[PRICE_THRESHOLD_INPUT];
    userInfo* user;

    get_user_input_graphic(orderList, userID, priceThresholdInput);
    printf("***Username: %s\n", userID);
        for (int i = 0; i < users_list->user_count; i++) {
        if (strcmp(users_list->users[i].userID, userID) == 0) {
            user_founded = 1;
            user = &users_list->users[i];
        }
    }
    if (!user_founded) {
        user = &users_list->users[users_list->user_count];
        user->store1_order_count = 0;
        user->store2_order_count = 1;
        user->store3_order_count = 0;
        users_list->user_count++;
    };

    if (strcmp(priceThresholdInput, "") == 0) priceThreshold = -1;
    else priceThreshold = atoi(priceThresholdInput);
    printf("Price threshold: %s %d\n", priceThresholdInput, priceThreshold);
    printf("OrderList: \n");
    for (int i = 0; i < ORDER_COUNT; i++) 
        printf("\t%s %d\n", orderList[i].name, orderList[i].count);


    // strcpy(userID, "user1");

    // for (int i = 0; i < users_list->user_count; i++) {
    //     if (strcmp(users_list->users[i].userID, userID) == 0) {
    //         user_founded = 1;
    //         user = &users_list->users[i];
    //     }
    // }
    // if (!user_founded) {
    //     user = &users_list->users[users_list->user_count];
    //     user->store1_order_count = 0;
    //     user->store2_order_count = 1;
    //     user->store3_order_count = 0;
    //     users_list->user_count++;
    // };

    // strcpy(orderList[0].name, "Jeans");
    // orderList[0].count = 2;
    
    // strcpy(orderList[1].name, "T-shirt");
    // orderList[1].count = 1;

    // strcpy(orderList[2].name, "Dress");
    // orderList[2].count = 2;
    
    // priceThreshold = 2200;

    // printf("Username: ");
    // scanf("%s", userID);

    // printf("OrderList: \n");
    // for (int i = 0; i < ORDER_COUNT; i++) scanf("%s %d", orderList[i].name, &orderList[i].count);

    // printf("Price threshold: \n");
    // getchar();
    // fgets(priceThresholdInput, sizeof(priceThresholdInput), stdin);
    // if (priceThresholdInput[0] == '\n') priceThreshold = -1;


    strcpy(user->userID, userID);
    for (int i = 0; i < ORDER_COUNT; i++) {
        strcpy(user->orderList[i].name, orderList[i].name);
        user->orderList[i].count = orderList[i].count;
    }
    user->priceThreshold = priceThreshold;
    return user;
}
