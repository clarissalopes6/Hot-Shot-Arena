#include "raylib.h"
#include "game.h"

int main(void) {

    InitWindow(1200, 800, "Hot Shot Arena");
    SetTargetFPS(60);

    Game game;
    InitGame(&game);

    while (!WindowShouldClose()) {
        UpdateGame(&game);
        DrawGame(&game);
    }

    UnloadGame(&game);
    CloseWindow();
    return 0;
}
