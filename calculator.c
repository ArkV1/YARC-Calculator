#include <raylib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>  // For pow function

#define WINDOW_WIDTH 300
#define WINDOW_HEIGHT 560  // Made taller for history
#define BUTTON_SIZE 60
#define PADDING 10
#define MAX_DIGITS 15
#define MAX_HISTORY 5
#define HISTORY_HEIGHT 100
#define MAX_EXPRESSION_LENGTH 256  // For storing the full expression
#define MAX_TOKENS 100

typedef struct {
    Rectangle rect;
    const char *label;
    bool is_operator;
    bool is_selected;  // New field to track if operator is selected
} Button;

typedef struct {
    char entries[MAX_HISTORY][64];
    int count;
} History;

typedef struct {
    double number;
    char operator;
    bool is_operator;
} Token;

char display_text[64] = "0";
char current_number[32] = "0";
char full_expression[MAX_EXPRESSION_LENGTH] = "";  // To store the complete expression
double first_number = 0;
int operation = 0;
bool new_number = true;
bool has_first_number = false;
History history = {0};

Button buttons[] = {
    // First row (new) - power
    {{PADDING + 0*(BUTTON_SIZE + PADDING), WINDOW_HEIGHT - 5*(BUTTON_SIZE + PADDING), BUTTON_SIZE, BUTTON_SIZE}, "^", true, false},
    
    // Original buttons moved down one row
    {{PADDING + 0*(BUTTON_SIZE + PADDING), WINDOW_HEIGHT - 4*(BUTTON_SIZE + PADDING), BUTTON_SIZE, BUTTON_SIZE}, "7", false, false},
    {{PADDING + 1*(BUTTON_SIZE + PADDING), WINDOW_HEIGHT - 4*(BUTTON_SIZE + PADDING), BUTTON_SIZE, BUTTON_SIZE}, "8", false, false},
    {{PADDING + 2*(BUTTON_SIZE + PADDING), WINDOW_HEIGHT - 4*(BUTTON_SIZE + PADDING), BUTTON_SIZE, BUTTON_SIZE}, "9", false, false},
    {{PADDING + 3*(BUTTON_SIZE + PADDING), WINDOW_HEIGHT - 4*(BUTTON_SIZE + PADDING), BUTTON_SIZE, BUTTON_SIZE}, "/", true, false},
    
    {{PADDING + 0*(BUTTON_SIZE + PADDING), WINDOW_HEIGHT - 3*(BUTTON_SIZE + PADDING), BUTTON_SIZE, BUTTON_SIZE}, "4", false, false},
    {{PADDING + 1*(BUTTON_SIZE + PADDING), WINDOW_HEIGHT - 3*(BUTTON_SIZE + PADDING), BUTTON_SIZE, BUTTON_SIZE}, "5", false, false},
    {{PADDING + 2*(BUTTON_SIZE + PADDING), WINDOW_HEIGHT - 3*(BUTTON_SIZE + PADDING), BUTTON_SIZE, BUTTON_SIZE}, "6", false, false},
    {{PADDING + 3*(BUTTON_SIZE + PADDING), WINDOW_HEIGHT - 3*(BUTTON_SIZE + PADDING), BUTTON_SIZE, BUTTON_SIZE}, "*", true, false},
    
    {{PADDING + 0*(BUTTON_SIZE + PADDING), WINDOW_HEIGHT - 2*(BUTTON_SIZE + PADDING), BUTTON_SIZE, BUTTON_SIZE}, "1", false, false},
    {{PADDING + 1*(BUTTON_SIZE + PADDING), WINDOW_HEIGHT - 2*(BUTTON_SIZE + PADDING), BUTTON_SIZE, BUTTON_SIZE}, "2", false, false},
    {{PADDING + 2*(BUTTON_SIZE + PADDING), WINDOW_HEIGHT - 2*(BUTTON_SIZE + PADDING), BUTTON_SIZE, BUTTON_SIZE}, "3", false, false},
    {{PADDING + 3*(BUTTON_SIZE + PADDING), WINDOW_HEIGHT - 2*(BUTTON_SIZE + PADDING), BUTTON_SIZE, BUTTON_SIZE}, "-", true, false},
    
    {{PADDING + 0*(BUTTON_SIZE + PADDING), WINDOW_HEIGHT - 1*(BUTTON_SIZE + PADDING), BUTTON_SIZE, BUTTON_SIZE}, "C", true, false},
    {{PADDING + 1*(BUTTON_SIZE + PADDING), WINDOW_HEIGHT - 1*(BUTTON_SIZE + PADDING), BUTTON_SIZE, BUTTON_SIZE}, "0", false, false},
    {{PADDING + 2*(BUTTON_SIZE + PADDING), WINDOW_HEIGHT - 1*(BUTTON_SIZE + PADDING), BUTTON_SIZE, BUTTON_SIZE}, "=", true, false},
    {{PADDING + 3*(BUTTON_SIZE + PADDING), WINDOW_HEIGHT - 1*(BUTTON_SIZE + PADDING), BUTTON_SIZE, BUTTON_SIZE}, "+", true, false}
};

void add_to_history(const char* expression, double result) {
    // Shift all entries down
    for (int i = MAX_HISTORY - 1; i > 0; i--) {
        strcpy(history.entries[i], history.entries[i-1]);
    }
    
    // Add new entry at the top
    snprintf(history.entries[0], sizeof(history.entries[0]), "%s = %.6g", expression, result);
    if (history.count < MAX_HISTORY) history.count++;
}

void update_display() {
    if (strlen(full_expression) == 0) {
        strcpy(display_text, current_number);
    } else {
        if (new_number) {
            strcpy(display_text, full_expression);
        } else {
            snprintf(display_text, sizeof(display_text), "%s %s", full_expression, current_number);
        }
    }
}

void handle_number(const char* num) {
    if (new_number) {
        strcpy(current_number, num);
        new_number = false;
    } else if (strlen(current_number) < MAX_DIGITS) {
        strcat(current_number, num);
    }
    update_display();
}

// Function to clear operator selections
void clear_operator_selections() {
    for (int i = 0; i < sizeof(buttons)/sizeof(buttons[0]); i++) {
        if (buttons[i].is_operator) {
            buttons[i].is_selected = false;
        }
    }
}

void handle_operator(const char* op) {
    if (strlen(full_expression) == 0) {
        // First operation
        snprintf(full_expression, sizeof(full_expression), "%s %s", current_number, op);
    } else if (!new_number) {
        // Append current number and operator to expression
        snprintf(full_expression, sizeof(full_expression), "%s %s %s", 
                full_expression, current_number, op);
    } else {
        // Just change the operator at the end
        full_expression[strlen(full_expression) - 1] = op[0];
    }
    
    new_number = true;
    
    // Clear previous operator selection
    clear_operator_selections();
    
    // Set the new operator and highlight it
    switch(op[0]) {
        case '+': operation = 1; break;
        case '-': operation = 2; break;
        case '*': operation = 3; break;
        case '/': operation = 4; break;
        case '^': operation = 5; break;
    }
    
    // Find and highlight the selected operator button
    for (int i = 0; i < sizeof(buttons)/sizeof(buttons[0]); i++) {
        if (buttons[i].is_operator && strcmp(buttons[i].label, op) == 0) {
            buttons[i].is_selected = true;
            break;
        }
    }
    
    update_display();
}

void handle_equals() {
    if (strlen(full_expression) == 0) return;  // Nothing to calculate
    
    // Add the final number to expression for history
    char final_expression[MAX_EXPRESSION_LENGTH];
    if (!new_number) {
        snprintf(final_expression, sizeof(final_expression), "%s %s", full_expression, current_number);
    } else {
        strcpy(final_expression, full_expression);
        // Remove the trailing operator
        final_expression[strlen(final_expression) - 1] = '\0';
    }
    
    // Tokenize the expression
    Token tokens[MAX_TOKENS];
    int token_count = 0;
    
    char *expr = strdup(final_expression);
    char *token = strtok(expr, " ");
    
    while (token != NULL && token_count < MAX_TOKENS) {
        if (token[0] == '+' || token[0] == '-' || token[0] == '*' || token[0] == '/' || token[0] == '^') {
            tokens[token_count].is_operator = true;
            tokens[token_count].operator = token[0];
        } else {
            tokens[token_count].is_operator = false;
            tokens[token_count].number = atof(token);
        }
        token_count++;
        token = strtok(NULL, " ");
    }
    free(expr);
    
    // Process operators in order of precedence: ^, then *, /, then +, -
    
    // First pass: process power (^) operations
    for (int i = 0; i < token_count - 1; i++) {
        if (tokens[i].is_operator && tokens[i].operator == '^') {
            if (i > 0 && i < token_count - 1) {
                double base = tokens[i-1].number;
                double exponent = tokens[i+1].number;
                double result = pow(base, exponent);
                
                if (isnan(result) || isinf(result)) {
                    strcpy(display_text, "Error");
                    strcpy(current_number, "0");
                    full_expression[0] = '\0';
                    new_number = true;
                    clear_operator_selections();
                    return;
                }
                
                // Replace the three tokens with the result
                tokens[i-1].number = result;
                tokens[i-1].is_operator = false;
                
                // Shift remaining tokens left
                for (int j = i; j < token_count - 2; j++) {
                    tokens[j] = tokens[j+2];
                }
                token_count -= 2;
                i--; // Recheck current position
            }
        }
    }
    
    // Second pass: process multiplication and division
    for (int i = 0; i < token_count - 1; i++) {
        if (tokens[i].is_operator && (tokens[i].operator == '*' || tokens[i].operator == '/')) {
            if (i > 0 && i < token_count - 1) {
                double left = tokens[i-1].number;
                double right = tokens[i+1].number;
                double result;
                
                if (tokens[i].operator == '*') {
                    result = left * right;
                } else {
                    if (right == 0) {
                        strcpy(display_text, "Error");
                        strcpy(current_number, "0");
                        full_expression[0] = '\0';
                        new_number = true;
                        clear_operator_selections();
                        return;
                    }
                    result = left / right;
                }
                
                // Replace the three tokens with the result
                tokens[i-1].number = result;
                tokens[i-1].is_operator = false;
                
                // Shift remaining tokens left
                for (int j = i; j < token_count - 2; j++) {
                    tokens[j] = tokens[j+2];
                }
                token_count -= 2;
                i--; // Recheck current position
            }
        }
    }
    
    // Final pass: process addition and subtraction
    double result = tokens[0].number;
    for (int i = 1; i < token_count - 1; i += 2) {
        double right = tokens[i+1].number;
        if (tokens[i].operator == '+') {
            result += right;
        } else if (tokens[i].operator == '-') {
            result -= right;
        }
    }
    
    // Add to history
    add_to_history(final_expression, result);
    
    // Update display
    snprintf(current_number, sizeof(current_number), "%.6g", result);
    strcpy(display_text, current_number);
    full_expression[0] = '\0';  // Clear the expression
    new_number = true;
    operation = 0;
    clear_operator_selections();
}

void handle_clear() {
    strcpy(display_text, "0");
    strcpy(current_number, "0");
    full_expression[0] = '\0';  // Clear the expression
    first_number = 0;
    operation = 0;
    new_number = true;
    has_first_number = false;
    clear_operator_selections();
}

int main(void) {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Calculator");
    SetTargetFPS(60);

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

        // Draw history
        DrawRectangle(PADDING, PADDING, WINDOW_WIDTH - 2*PADDING, HISTORY_HEIGHT, WHITE);
        DrawRectangleLines(PADDING, PADDING, WINDOW_WIDTH - 2*PADDING, HISTORY_HEIGHT, LIGHTGRAY);
        for (int i = 0; i < history.count; i++) {
            DrawText(history.entries[i], PADDING + 5, PADDING + 5 + (i * 20), 12, GRAY);
        }

        // Draw current expression display
        DrawRectangle(PADDING, HISTORY_HEIGHT + 2*PADDING, WINDOW_WIDTH - 2*PADDING, 50, WHITE);
        DrawRectangleLines(PADDING, HISTORY_HEIGHT + 2*PADDING, WINDOW_WIDTH - 2*PADDING, 50, LIGHTGRAY);
        DrawText(display_text, WINDOW_WIDTH - PADDING - MeasureText(display_text, 20) - 5, HISTORY_HEIGHT + 25, 20, BLACK);

        // Draw buttons
        for (int i = 0; i < sizeof(buttons)/sizeof(buttons[0]); i++) {
            Color buttonColor = WHITE;
            if (buttons[i].is_operator) {
                buttonColor = buttons[i].is_selected ? ORANGE : LIGHTGRAY;
            }
            DrawRectangleRec(buttons[i].rect, buttonColor);
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