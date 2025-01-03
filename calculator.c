#include <raylib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>  // For pow function

#define WINDOW_WIDTH 300
#define WINDOW_HEIGHT 460  // Made window taller for extra row
#define BUTTON_SIZE 60
#define PADDING 10
#define MAX_DIGITS 15

typedef struct {
    Rectangle rect;
    const char *label;
    bool is_operator;
} Button;

char display_text[32] = "0";
double first_number = 0;
int operation = 0; // 0: none, 1: add, 2: sub, 3: mul, 4: div, 5: pow
bool new_number = true;

Button buttons[] = {
    // First row (new) - power
    {{PADDING + 0*(BUTTON_SIZE + PADDING), WINDOW_HEIGHT - 5*(BUTTON_SIZE + PADDING), BUTTON_SIZE, BUTTON_SIZE}, "^", true},
    
    // Original buttons moved down one row
    {{PADDING + 0*(BUTTON_SIZE + PADDING), WINDOW_HEIGHT - 4*(BUTTON_SIZE + PADDING), BUTTON_SIZE, BUTTON_SIZE}, "7", false},
    {{PADDING + 1*(BUTTON_SIZE + PADDING), WINDOW_HEIGHT - 4*(BUTTON_SIZE + PADDING), BUTTON_SIZE, BUTTON_SIZE}, "8", false},
    {{PADDING + 2*(BUTTON_SIZE + PADDING), WINDOW_HEIGHT - 4*(BUTTON_SIZE + PADDING), BUTTON_SIZE, BUTTON_SIZE}, "9", false},
    {{PADDING + 3*(BUTTON_SIZE + PADDING), WINDOW_HEIGHT - 4*(BUTTON_SIZE + PADDING), BUTTON_SIZE, BUTTON_SIZE}, "/", true},
    
    {{PADDING + 0*(BUTTON_SIZE + PADDING), WINDOW_HEIGHT - 3*(BUTTON_SIZE + PADDING), BUTTON_SIZE, BUTTON_SIZE}, "4", false},
    {{PADDING + 1*(BUTTON_SIZE + PADDING), WINDOW_HEIGHT - 3*(BUTTON_SIZE + PADDING), BUTTON_SIZE, BUTTON_SIZE}, "5", false},
    {{PADDING + 2*(BUTTON_SIZE + PADDING), WINDOW_HEIGHT - 3*(BUTTON_SIZE + PADDING), BUTTON_SIZE, BUTTON_SIZE}, "6", false},
    {{PADDING + 3*(BUTTON_SIZE + PADDING), WINDOW_HEIGHT - 3*(BUTTON_SIZE + PADDING), BUTTON_SIZE, BUTTON_SIZE}, "*", true},
    
    {{PADDING + 0*(BUTTON_SIZE + PADDING), WINDOW_HEIGHT - 2*(BUTTON_SIZE + PADDING), BUTTON_SIZE, BUTTON_SIZE}, "1", false},
    {{PADDING + 1*(BUTTON_SIZE + PADDING), WINDOW_HEIGHT - 2*(BUTTON_SIZE + PADDING), BUTTON_SIZE, BUTTON_SIZE}, "2", false},
    {{PADDING + 2*(BUTTON_SIZE + PADDING), WINDOW_HEIGHT - 2*(BUTTON_SIZE + PADDING), BUTTON_SIZE, BUTTON_SIZE}, "3", false},
    {{PADDING + 3*(BUTTON_SIZE + PADDING), WINDOW_HEIGHT - 2*(BUTTON_SIZE + PADDING), BUTTON_SIZE, BUTTON_SIZE}, "-", true},
    
    {{PADDING + 0*(BUTTON_SIZE + PADDING), WINDOW_HEIGHT - 1*(BUTTON_SIZE + PADDING), BUTTON_SIZE, BUTTON_SIZE}, "C", true},
    {{PADDING + 1*(BUTTON_SIZE + PADDING), WINDOW_HEIGHT - 1*(BUTTON_SIZE + PADDING), BUTTON_SIZE, BUTTON_SIZE}, "0", false},
    {{PADDING + 2*(BUTTON_SIZE + PADDING), WINDOW_HEIGHT - 1*(BUTTON_SIZE + PADDING), BUTTON_SIZE, BUTTON_SIZE}, "=", true},
    {{PADDING + 3*(BUTTON_SIZE + PADDING), WINDOW_HEIGHT - 1*(BUTTON_SIZE + PADDING), BUTTON_SIZE, BUTTON_SIZE}, "+", true}
};

void handle_number(const char* num) {
    if (new_number) {
        strcpy(display_text, num);
        new_number = false;
    } else if (strlen(display_text) < MAX_DIGITS) {
        strcat(display_text, num);
    }
}

void handle_operator(const char* op) {
    first_number = atof(display_text);
    new_number = true;
    
    switch(op[0]) {
        case '+': operation = 1; break;
        case '-': operation = 2; break;
        case '*': operation = 3; break;
        case '/': operation = 4; break;
        case '^': operation = 5; break;
    }
}

void handle_equals() {
    double second_number = atof(display_text);
    double result = 0;
    
    switch(operation) {
        case 1: result = first_number + second_number; break;
        case 2: result = first_number - second_number; break;
        case 3: result = first_number * second_number; break;
        case 4:
            if (second_number != 0) {
                result = first_number / second_number;
            } else {
                strcpy(display_text, "Error");
                new_number = true;
                return;
            }
            break;
        case 5:
            result = pow(first_number, second_number);
            if (isnan(result) || isinf(result)) {
                strcpy(display_text, "Error");
                new_number = true;
                return;
            }
            break;
    }
    
    snprintf(display_text, sizeof(display_text), "%.6g", result);
    new_number = true;
    operation = 0;
}

void handle_clear() {
    strcpy(display_text, "0");
    first_number = 0;
    operation = 0;
    new_number = true;
}

int main(void) {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Calculator");
    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose()) {
        // Update
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 mousePoint = GetMousePosition();
            
            for (int i = 0; i < sizeof(buttons)/sizeof(buttons[0]); i++) {
                if (CheckCollisionPointRec(mousePoint, buttons[i].rect)) {
                    if (strcmp(buttons[i].label, "=") == 0) {
                        handle_equals();
                    } else if (strcmp(buttons[i].label, "C") == 0) {
                        handle_clear();
                    } else if (buttons[i].is_operator) {
                        handle_operator(buttons[i].label);
                    } else {
                        handle_number(buttons[i].label);
                    }
                    break;
                }
            }
        }

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw display
        DrawRectangle(PADDING, PADDING, WINDOW_WIDTH - 2*PADDING, 50, WHITE);
        DrawRectangleLines(PADDING, PADDING, WINDOW_WIDTH - 2*PADDING, 50, LIGHTGRAY);
        DrawText(display_text, WINDOW_WIDTH - PADDING - MeasureText(display_text, 20) - 5, 25, 20, BLACK);

        // Draw buttons
        for (int i = 0; i < sizeof(buttons)/sizeof(buttons[0]); i++) {
            DrawRectangleRec(buttons[i].rect, buttons[i].is_operator ? LIGHTGRAY : WHITE);
            DrawRectangleLinesEx(buttons[i].rect, 1, GRAY);
            
            int textWidth = MeasureText(buttons[i].label, 20);
            Vector2 textPos = {
                buttons[i].rect.x + (buttons[i].rect.width - textWidth)/2,
                buttons[i].rect.y + (buttons[i].rect.height - 20)/2
            };
            DrawText(buttons[i].label, textPos.x, textPos.y, 20, BLACK);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
} 