#include "raylib.h"
#include "game.h"

int main(void) {

    InitWindow(0, 0, "Hot Shot Arena");
    ToggleFullscreen();
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
