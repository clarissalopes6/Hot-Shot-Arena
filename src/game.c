#include "raylib.h"
#include "game.h"

Texture2D campo;
Texture2D player;
Vector2 playerPos = { 500, 500 };

void InitGame() {
    campo = LoadTexture("assets/quadra.png");
    player = LoadTexture("assets/player.png");
}

void UpdateGame() {
    if (IsKeyDown(KEY_RIGHT)) playerPos.x += 5;
    if (IsKeyDown(KEY_LEFT)) playerPos.x -= 5;
    if (IsKeyDown(KEY_UP)) playerPos.y -= 5;
    if (IsKeyDown(KEY_DOWN)) playerPos.y += 5;

    if (playerPos.x < 0) playerPos.x = 0;
    if (playerPos.y < 0) playerPos.y = 0;
    if (playerPos.x > GetScreenWidth() - player.width) playerPos.x = GetScreenWidth() - player.width;
    if (playerPos.y > GetScreenHeight() - player.height) playerPos.y = GetScreenHeight() - player.height;
}

void DrawGame() {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    DrawTexture(campo, 0, 0, WHITE);
    DrawTexture(player, playerPos.x, playerPos.y, WHITE);

    EndDrawing();
}

void UnloadGame() {
    UnloadTexture(campo);
    UnloadTexture(player);
}
