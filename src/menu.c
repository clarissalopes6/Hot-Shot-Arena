#include "raylib.h"
#include "menu.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define BUTTON_WIDTH 200
#define BUTTON_HEIGHT 60

MenuState draw_initial_menu(Assets *assets) {
    Rectangle start_btn = {
        SCREEN_WIDTH/2 - BUTTON_WIDTH/2, 
        SCREEN_HEIGHT/2 - 100, 
        BUTTON_WIDTH, 
        BUTTON_HEIGHT
    };
    Rectangle instructions_btn = {
        SCREEN_WIDTH/2 - BUTTON_WIDTH/2, 
        SCREEN_HEIGHT/2, 
        BUTTON_WIDTH, 
        BUTTON_HEIGHT
    };
    
    BeginDrawing();
    ClearBackground(BLACK);
    
    if (assets->menu.id > 0) {
        DrawTexturePro(assets->menu, 
                       (Rectangle){0, 0, assets->menu.width, assets->menu.height},
                       (Rectangle){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT},
                       (Vector2){0, 0}, 0, WHITE);
    }
    
    Color start_color = CheckCollisionPointRec(GetMousePosition(), start_btn) ? YELLOW : WHITE;
    Color instr_color = CheckCollisionPointRec(GetMousePosition(), instructions_btn) ? YELLOW : WHITE;
    
    DrawRectangleRec(start_btn, (Color){0, 0, 0, 150});
    DrawRectangleLinesEx(start_btn, 3, start_color);
    DrawText("Começar Jogo", start_btn.x + 15, start_btn.y + 15, 20, start_color);
    
    DrawRectangleRec(instructions_btn, (Color){0, 0, 0, 150});
    DrawRectangleLinesEx(instructions_btn, 3, instr_color);
    DrawText("Instruções", instructions_btn.x + 35, instructions_btn.y + 15, 20, instr_color);
    
    EndDrawing();
    
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(GetMousePosition(), start_btn)) {
            return MENU_INITIAL;
        }
        if (CheckCollisionPointRec(GetMousePosition(), instructions_btn)) {
            return MENU_INSTRUCTIONS;
        }
    }
    
    return -1;
}

MenuState draw_instructions_menu(Assets *assets) {
    Rectangle back_btn = {
        SCREEN_WIDTH/2 - BUTTON_WIDTH/2, 
        SCREEN_HEIGHT - 120, 
        BUTTON_WIDTH, 
        BUTTON_HEIGHT
    };
    
    BeginDrawing();
    ClearBackground(BLACK);
    
    if (assets->instructions.id > 0) {
        DrawTexturePro(assets->instructions,
                       (Rectangle){0, 0, assets->instructions.width, assets->instructions.height},
                       (Rectangle){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT},
                       (Vector2){0, 0}, 0, WHITE);
    }
    
    Color back_color = CheckCollisionPointRec(GetMousePosition(), back_btn) ? YELLOW : WHITE;
    DrawRectangleRec(back_btn, (Color){0, 0, 0, 150});
    DrawRectangleLinesEx(back_btn, 3, back_color);
    DrawText("Voltar", back_btn.x + 55, back_btn.y + 15, 20, back_color);
    
    EndDrawing();
    
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(GetMousePosition(), back_btn)) {
            return MENU_INITIAL;
        }
    }
    
    return -1;
}
