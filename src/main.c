#include "raylib.h"
#include "game.h"

int main(void) {
    
    InitWindow(0, 0, "Hot Shot Arena");
    ToggleFullscreen();
    SetTargetFPS(60);

    InitGame(); 

    while (!WindowShouldClose()) {
        UpdateGame();  
        DrawGame();    
    }

    UnloadGame();  
    CloseWindow(); 
    return 0;
}
