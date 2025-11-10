#include "raylib.h"

int main(void) {
    InitWindow(0, 0, "Hot Shot Arena");

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Teste", 250, 200, 30, BLACK);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
