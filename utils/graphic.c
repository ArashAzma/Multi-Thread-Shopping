     
#include "raylib.h"
#include <string.h>
#include <stdlib.h>

#include "../headers/user.h"
#include "../headers/cat.h"
#include "../headers/graphic.h"

void get_user_input_graphic(order order_list[ORDER_COUNT], char username_input[USER_ID_LENGTH], char priceThreshold_input[PRICE_THRESHOLD_INPUT]) {
    ProductInfo names_array[MAX_CATEGORIES * MAX_FILES_PER_CATEGORY];
    int total_names = 0;

    if (!extract_names_from_files_graphic("Dataset/Store1", names_array, &total_names)) {
        printf("Failed to load names.");
        return -1;
    }

    CategoryGraphic categories[total_names];
    for (int i = 0; i < total_names; i++) {
        strncpy(categories[i].name, names_array[i].name, sizeof(categories[i].name));
        // printf("%s\n", categories[i].name);
        categories[i].isChecked = 0;
        categories[i].numberInput[0] = '\0';
        categories[i].isInputActive = 0;
    }

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