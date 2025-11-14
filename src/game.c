#include "raylib.h"
#include "game.h"

void InitGame(Game *game) {
    game->campo = LoadTexture("assets/quadra.png");
    game->player = InitPlayer();
}

void UpdateGame(Game *game) {
    float screenW = GetScreenWidth();
    float screenH = GetScreenHeight();

    float imgW = game->campo.width;
    float imgH = game->campo.height;

    float scale = screenH / imgH;
    float scaledW = imgW * scale;

    float offsetX = (screenW - scaledW) / 2;
    float minX = offsetX;
    float maxX = offsetX + scaledW;

    UpdatePlayer(&game->player, minX, maxX, screenH);
}

void DrawGame(Game *game) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    float screenW = GetScreenWidth();
    float screenH = GetScreenHeight();

    float imgW = game->campo.width;
    float imgH = game->campo.height;

    float scale = screenH / imgH;
    float scaledW = imgW * scale;
    float scaledH = imgH * scale;

    float offsetX = (screenW - scaledW) / 2;

    Rectangle source = { 0, 0, imgW, imgH };
    Rectangle dest   = { offsetX, 0, scaledW, scaledH };

    DrawTexturePro(game->campo, source, dest, (Vector2){0,0}, 0.0f, WHITE);

    DrawPlayer(&game->player);

    EndDrawing();
}

void UnloadGame(Game *game) {
    UnloadTexture(game->campo);
    UnloadPlayer(&game->player);
}
