     
#include "raylib.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "../headers/user.h"
#include "../headers/cat.h"
#include "../headers/graphic.h"
#include "../headers/process.h"

void get_user_input_graphic(order order_list[ORDER_COUNT], char username_input[USER_ID_LENGTH], char priceThreshold_input[PRICE_THRESHOLD_INPUT]) {
    ProductInfo names_array[MAX_CATEGORIES * MAX_FILES_PER_CATEGORY];
    int total_names = 0;

    if (!extract_names_from_files_graphic("DatasetTest/Store1", names_array, &total_names)) {
        printf("Failed to load names.");
        return -1;
    }

    CategoryGraphic categories[total_names];
    for (int i = 0; i < total_names; i++) {
        strncpy(categories[i].name, names_array[i].name, sizeof(categories[i].name));
        categories[i].isChecked = 0;
        categories[i].numberInput[0] = '\0';
        categories[i].isInputActive = 0;
    }
    SetTraceLogLevel(LOG_NONE);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "SALAM BRAR");
    SetTargetFPS(60);

    int scrollOffset = 0;

    Rectangle saveButton = { SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT - 50, 100, 30 };
    bool saveClicked = false;

    char username[50] = "";
    char priceThreshold[10] = "";
    bool usernameActive = false;
    bool priceThresholdActive = false;

    while (!WindowShouldClose()) {
        Vector2 mousePoint = GetMousePosition();

        scrollOffset += GetMouseWheelMove() * SCROLL_SPEED;

        if (scrollOffset < 0) scrollOffset = 0;
        if (scrollOffset > (total_names - MAX_DISPLAY_ITEMS) * 50) 
            scrollOffset = (total_names - MAX_DISPLAY_ITEMS) * 50;

        for (int i = 0; i < MAX_DISPLAY_ITEMS; i++) {
            int index = (scrollOffset / 50) + i;
            if (index >= total_names) break;

            Rectangle checkboxBounds = {50, 50 + i * 50, 20, 20};
            Rectangle inputBounds = {300, 50 + i * 50, 100, 30};

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
                CheckCollisionPointRec(mousePoint, checkboxBounds)) {
                categories[index].isChecked = !categories[index].isChecked;
            }

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
                CheckCollisionPointRec(mousePoint, inputBounds)) {
                for (int j = 0; j < total_names; j++) {
                    categories[j].isInputActive = 0;
                }
                categories[index].isInputActive = 1;
            }

            if (categories[index].isInputActive) {
                if (IsKeyPressed(KEY_BACKSPACE)) {
                    int len = strlen(categories[index].numberInput);
                    if (len > 0) {
                        categories[index].numberInput[len - 1] = '\0';
                    }
                }

                int key;
                while ((key = GetCharPressed()) != 0) {
                    if (key >= '0' && key <= '9' &&
                        strlen(categories[index].numberInput) < MAX_INPUT_CHARS) {
                        int len = strlen(categories[index].numberInput);
                        categories[index].numberInput[len] = (char)key;
                        categories[index].numberInput[len + 1] = '\0';
                    }
                }
            }
        }

        Rectangle usernameBox = {SCREEN_WIDTH - 250, 20, 200, 30};
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mousePoint, usernameBox)) {
            usernameActive = true;
            priceThresholdActive = false;
        }
        if (usernameActive) {
            if (IsKeyPressed(KEY_BACKSPACE)) {
                int len = strlen(username);
                if (len > 0) {
                    username[len - 1] = '\0';
                }
            }

            int key;
            while ((key = GetCharPressed()) != 0) {
                if ((key >= 'A' && key <= 'Z') || (key >= 'a' && key <= 'z') || key == ' ' ||
                    (key >= '0' && key <= '9') && strlen(username) < sizeof(username) - 1) {
                    int len = strlen(username);
                    username[len] = (char)key;
                    username[len + 1] = '\0';
                }
            }
        }

        Rectangle priceThresholdBox = {SCREEN_WIDTH - 250, 60, 200, 30};
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mousePoint, priceThresholdBox)) {
            usernameActive = false;
            priceThresholdActive = true;
        }
        if (priceThresholdActive) {
            if (IsKeyPressed(KEY_BACKSPACE)) {
                int len = strlen(priceThreshold);
                if (len > 0) {
                    priceThreshold[len - 1] = '\0';
                }
            }

            int key;
            while ((key = GetCharPressed()) != 0) {
                if (key >= '0' && key <= '9' && strlen(priceThreshold) < sizeof(priceThreshold) - 1) {
                    int len = strlen(priceThreshold);
                    priceThreshold[len] = (char)key;
                    priceThreshold[len + 1] = '\0';
                }
            }
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
            CheckCollisionPointRec(mousePoint, saveButton)) {
            saveClicked = true;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (int i = 0; i < MAX_DISPLAY_ITEMS; i++) {
            int index = (scrollOffset / 50) + i;
            if (index >= total_names) break;

            Color checkColor = categories[index].isChecked ? BLUE : GRAY;
            DrawRectangleLines(50, 50 + i * 50, 20, 20, BLACK);
            DrawRectangle(50, 50 + i * 50, 20, 20, checkColor);

            DrawText(categories[index].name, 80, 52 + i * 50, 20, BLACK);

            Color inputColor = categories[index].isInputActive ? LIGHTGRAY : DARKGRAY;
            DrawRectangleLines(300, 50 + i * 50, 100, 30, BLACK);
            DrawRectangle(300, 50 + i * 50, 100, 30, inputColor);

            DrawText(categories[index].numberInput, 310, 57 + i * 50, 20, BLACK);
        }

        Color usernameColor = usernameActive ? LIGHTGRAY : DARKGRAY;
        DrawRectangleLines(usernameBox.x, usernameBox.y, usernameBox.width, usernameBox.height, BLACK);
        DrawRectangle(usernameBox.x, usernameBox.y, usernameBox.width, usernameBox.height, usernameColor);
        DrawText(username, usernameBox.x + 5, usernameBox.y + 5, 20, BLACK);
        DrawText("Username:", usernameBox.x - 120, usernameBox.y + 5, 20, BLACK);

        Color priceColor = priceThresholdActive ? LIGHTGRAY : DARKGRAY;
        DrawRectangleLines(priceThresholdBox.x, priceThresholdBox.y, priceThresholdBox.width, priceThresholdBox.height, BLACK);
        DrawRectangle(priceThresholdBox.x, priceThresholdBox.y, priceThresholdBox.width, priceThresholdBox.height, priceColor);
        DrawText(priceThreshold, priceThresholdBox.x + 5, priceThresholdBox.y + 5, 20, BLACK);
        DrawText("Price Threshold:", priceThresholdBox.x - 180, priceThresholdBox.y + 5, 20, BLACK);

        DrawRectangleRec(saveButton, LIGHTGRAY);
        DrawRectangleLines(saveButton.x, saveButton.y, saveButton.width, saveButton.height, BLACK);
        DrawText("Save", saveButton.x + 25, saveButton.y + 5, 20, BLACK);

        float indicatorHeight = (float)SCREEN_HEIGHT / total_names;
        DrawRectangle(SCREEN_WIDTH - 20, scrollOffset * SCREEN_HEIGHT / (total_names * 50), 10, indicatorHeight, DARKGRAY);

        EndDrawing();
        if(saveClicked) break;
    }

    CloseWindow();

    int c = 0;
    for (int i = 0; i < total_names; i++) {
        if(categories[i].isChecked){
            strcpy(order_list[c].name, categories[i].name);
            order_list[c].count = atoi(categories[i].numberInput);
            c++;
        }

    }
    strcpy(username_input, username);
    strcpy(priceThreshold_input, priceThreshold);
    return 0;
}

void displayFinalOrderText(const char* text, int store_index, const char* user_id) {
    SetTraceLogLevel(LOG_NONE);
    InitWindow(800, 450, "Order Confirmation");
    
    char display_text[256];
    snprintf(display_text, sizeof(display_text), 
             "Store%d\nis the Best order for %s\n", 
             store_index + 1, user_id);
    
    SetTargetFPS(20);

    while (!WindowShouldClose()) {
        BeginDrawing();
        
        ClearBackground(DARKBLUE);
        
        DrawText(display_text, 100, 200, 20, WHITE);
        
        EndDrawing();
    }

    CloseWindow();
}

typedef struct {
    char* itemName;  
    int score;
    bool isScoreEntered;
} ItemScore;

void handle_store_scores(char** names, int* scores, int order_count) {
    if (names == NULL || scores == NULL || order_count <= 0) {
        fprintf(stderr, "Invalid input to handle_store_scores\n");
        return;
    }
    SetTraceLogLevel(LOG_NONE);
    InitWindow(800, 600, "Store Scores Input");
    SetTargetFPS(60);

    ItemScore* items = calloc(order_count, sizeof(ItemScore));
    if (items == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        CloseWindow();
        return;
    }

    for(int i = 0; i < order_count; i++) {
        items[i].itemName = strdup(names[i]);
        items[i].score = -1;
        items[i].isScoreEntered = false;
    }

    int current_item = 0;
    char input_text[10] = "";

    while (!WindowShouldClose() && current_item < order_count) {
        BeginDrawing();
        ClearBackground(WHITE);

        char prompt[256];
        snprintf(prompt, sizeof(prompt), 
                 "Enter score for %s (0-10):", 
                 items[current_item].itemName);
        
        DrawText(prompt, 50, 200, 20, BLACK);
        DrawText("Use numbers and ENTER to confirm", 50, 230, 16, GRAY);

        DrawText(input_text, 50, 270, 20, BLUE);

        int key = GetCharPressed();
        if (key >= '0' && key <= '9' && strlen(input_text) < 2) {
            int len = strlen(input_text);
            input_text[len] = key;
            input_text[len + 1] = '\0';
        }

        if (IsKeyPressed(KEY_BACKSPACE)) {
            int len = strlen(input_text);
            if (len > 0) {
                input_text[len - 1] = '\0';
            }
        }

        if (IsKeyPressed(KEY_ENTER)) {
            int user_score = atoi(input_text);
            
            if (user_score >= 0 && user_score <= 10) {
                items[current_item].score = user_score;
                items[current_item].isScoreEntered = true;
                
                current_item++;
                
                memset(input_text, 0, sizeof(input_text));
            } else {
                memset(input_text, 0, sizeof(input_text));
            }
        }

        DrawText("Entered Scores:", 50, 350, 18, DARKGREEN);
        for (int i = 0; i < order_count; i++) {
            char score_text[100];
            if (items[i].isScoreEntered) {
                snprintf(score_text, sizeof(score_text), 
                         "%s: %d", items[i].itemName, items[i].score);
                DrawText(score_text, 50, 380 + i * 30, 16, BLACK);
            } else if (i == current_item) {
                snprintf(score_text, sizeof(score_text), 
                         "%s: Entering...", items[i].itemName);
                DrawText(score_text, 50, 380 + i * 30, 16, BLUE);
            } else {
                snprintf(score_text, sizeof(score_text), 
                         "%s: Not entered", items[i].itemName);
                DrawText(score_text, 50, 380 + i * 30, 16, GRAY);
            }
        }

        if (current_item >= order_count) {
            DrawText("All item scores entered!", 50, 500, 20, GREEN);
        }

        EndDrawing();
    }

    CloseWindow();
    for (int i = 0; i < order_count; i++) {
        scores[i] = items[i].score;
        
        free(items[i].itemName);
    }

    free(items);
}