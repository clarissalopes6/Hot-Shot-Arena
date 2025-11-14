#include "raylib.h"
#include "game.h"
#include "ball.h"

void InitGame(Game *game) {
    game->campo = LoadTexture("assets/quadra.png");
    game->player = InitPlayer();
    game->ball = InitBall();
    
    float screenW = GetScreenWidth();
    float screenH = GetScreenHeight();
    game->ball.position = (Vector2){screenW / 2, screenH / 2};
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
    
    if (!game->player.hasBall && game->player.ballPickupCooldown <= 0 && 
        CheckBallPlayerCollision(&game->ball, 
                                game->player.position, 
                                game->player.frameWidth * game->player.scale, 
                                game->player.frameHeight * game->player.scale)) {
        game->player.hasBall = true;
        game->ball.isBeingCarried = true;
        game->ball.isVisible = false;
    }
    
    static bool wasChargingLastFrame = false;
    if (game->player.hasBall && wasChargingLastFrame && !game->player.isCharging) {
        ThrowBall(&game->ball, game->player.position, game->player.facing, game->player.throwForce);
        game->player.hasBall = false;
        game->player.throwForce = 0.0f;
    }
    wasChargingLastFrame = game->player.isCharging;
    
    UpdateBall(&game->ball);
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

    DrawBall(&game->ball);
    DrawPlayer(&game->player);

    if (!game->player.hasBall) {
        if (game->player.ballPickupCooldown > 0) {
            DrawText("Aguarde para pegar a bola novamente", 10, 10, 20, RED);
        } else {
            DrawText("Chegue perto da bola para pegá-la", 10, 10, 20, WHITE);
        }
    } else {
        DrawText("Segure Z para carregar força e lance a bola", 10, 10, 20, WHITE);
    }
    
    float midX = offsetX + scaledW / 2;
    DrawLine(midX, 0, midX, screenH, (Color){255, 255, 255, 100});

    EndDrawing();
}

void UnloadGame(Game *game) {
    UnloadTexture(game->campo);
    UnloadPlayer(&game->player);
    UnloadBall(&game->ball);
}
