#ifndef GRAPHIC_H
#define GRAPHIC_H

#include "user.h"
#include "process.h"

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 600
#define MAX_CATEGORIES 10
#define MAX_INPUT_CHARS 5

#define MAX_DISPLAY_ITEMS 10
#define SCROLL_SPEED 10

typedef struct {
    char name[50];
    int isChecked;
    char numberInput[MAX_INPUT_CHARS + 1];
    int isInputActive;
} CategoryGraphic;

void get_user_input_graphic(order order_list[ORDER_COUNT], char username_input[USER_ID_LENGTH], char priceThreshold_input[PRICE_THRESHOLD_INPUT]);
void displayFinalOrderText(const char* text, int store_index, const char* user_id);
void handle_store_scores(char** names, int* scores, int order_count, char userID[20]);

#endif