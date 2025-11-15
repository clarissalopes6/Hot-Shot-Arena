#include "raylib.h"
#include "game.h"
#include "ball.h"
#include "list.h"
#include "bot.h"
#include "raymath.h"
#include "sort.h"

void eliminarJogador(Player *player, Queue *waitQueue, Ball *ball) {
    if (player->state == STATE_HIT) return; 

    Enqueue(waitQueue, *player);
    
    player->position = (Vector2){-500,-500}; 
    player->state = STATE_HIT; 
}

void resgatarJogador(Player *player, Queue *waitQueue, Vector2 spawnPos) {
    if (IsQueueEmpty(waitQueue)) return;
    
    Player rescuedPlayer = Dequeue(waitQueue);
    
    *player = rescuedPlayer;
    player->position = spawnPos;
    player->state = STATE_IDLE;
    player->hasBall = false;
    player->throwForce = 0.0f;
    player->isCharging = false;
    player->ballPickupCooldown = 1.0f; 
}

void exibirRanking(Game *game, float offsetX, int screenH) {
    Player allPlayers[4];
    
    allPlayers[0] = game->player;
    allPlayers[0].score = game->player.score;
    
    for (int i = 0; i < 3; i++) {
        allPlayers[i+1] = game->opponent[i];
    }

    BubbleSortPlayers(allPlayers, 4);

    DrawText("RANKING (Acertos):", offsetX + 10, screenH - 120, 20, LIGHTGRAY);
    
    for (int i = 0; i < 4; i++) {
        const char *name;
        if (allPlayers[i].score == game->player.score && i == 0) { 
            name = "Player 1";
        } else {
            name = TextFormat("Bot %d", i + 1);
        }
        DrawText(TextFormat("%d. %s - Acertos: %d", i + 1, name, allPlayers[i].score), 
                 offsetX + 10, screenH - 100 + (i * 20), 18, (i == 0) ? GOLD : WHITE);
    }
}

void InitGame(Game *game) {
    game->campo = LoadTexture("assets/quadra.png");
    game->player = InitPlayer();
    game->ball = InitBall();
    
    game->team1_wait_queue = InitQueue();
    game->team2_wait_queue = InitQueue();

    game->score_team1 = 0;
    game->score_team2 = 0;
    
    float screenW = GetScreenWidth();
    float screenH = GetScreenHeight();
    
    float offsetX = (screenW - (game->campo.width * screenH / game->campo.height)) / 2;
    float minX = offsetX;
    float maxX = offsetX + (game->campo.width * screenH / game->campo.height);
    float midX = (minX + maxX) / 2;
    
    game->ball.position = (Vector2){midX - 100, screenH / 2}; 
    
   
    game->opponent[0] = InitBot((Vector2){midX + 100, 300}, 2);
    game->opponent[1] = InitBot((Vector2){midX + 200, 500}, 2);
    game->opponent[2] = InitBot((Vector2){midX + 150, 700}, 2);
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
    float midX = (minX + maxX) / 2;
    float maxQuadraY = screenH;

    UpdatePlayer(&game->player, minX, midX, maxQuadraY); 
    
    if (game->player.ballPickupCooldown > 0) {
        game->player.ballPickupCooldown -= GetFrameTime();
        if (game->player.ballPickupCooldown < 0) {
            game->player.ballPickupCooldown = 0;
        }
    }
    
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
        game->player.ballPickupCooldown = 1.0f;
    }
    wasChargingLastFrame = game->player.isCharging;
    
    
    for (int i = 0; i < 3; i++) {
        Player *currentOpponent = &game->opponent[i];

        UpdateBot(currentOpponent, &game->ball, midX, maxX, maxQuadraY);
        
        static bool wasBotChargingLastFrame[3] = {false};
        if (currentOpponent->hasBall && wasBotChargingLastFrame[i] && !currentOpponent->isCharging) {
            ThrowBall(&game->ball, currentOpponent->position, currentOpponent->facing, currentOpponent->throwForce);
            currentOpponent->hasBall = false;
            currentOpponent->throwForce = 0.0f;
            currentOpponent->ballPickupCooldown = 1.0f;
        }
        wasBotChargingLastFrame[i] = currentOpponent->isCharging;

        if (currentOpponent->ballPickupCooldown > 0) {
            currentOpponent->ballPickupCooldown -= GetFrameTime();
            if (currentOpponent->ballPickupCooldown < 0) {
                currentOpponent->ballPickupCooldown = 0;
            }
        }
    }

    bool wasHit = false;

    if (!game->ball.isBeingCarried && game->ball.z < 20.0f) {
        
       
        for (int i = 0; i < 3; i++) {
            Player *currentOpponent = &game->opponent[i];
            
            if (currentOpponent->state != STATE_HIT && 
                CheckCollisionCircleRec(game->ball.position, game->ball.radius * game->ball.scale, 
                                        (Rectangle){currentOpponent->position.x, 
                                                    currentOpponent->position.y, 
                                                    currentOpponent->frameWidth * currentOpponent->scale, 
                                                    currentOpponent->frameHeight * currentOpponent->scale})) {
                
                eliminarJogador(currentOpponent, game->team2_wait_queue, &game->ball);
                game->player.score++; 
                game->score_team1++;
                
                Vector2 resgatePos = {minX , screenH / 2}; 
                
                for (int j = 0; j < 3; j++) {
                    if (game->opponent[j].state == STATE_HIT) {
                        resgatarJogador(&game->opponent[j], game->team2_wait_queue, resgatePos);
                        game->opponent[j].hasBall = true;
                        
                        game->ball.position = resgatePos; 
                        game->ball.velocity = (Vector2){0, 0};
                        game->ball.z = 0;
                        game->ball.isBeingCarried = true;
                        game->ball.isVisible = false;
                        break;
                    }
                }

                wasHit = true; 
                break; 
            }
        }
        
   
    }
    
    if (!game->ball.isBeingCarried && !wasHit) {
    
        if (!game->player.hasBall && game->player.ballPickupCooldown <= 0 && 
            CheckBallPlayerCollision(&game->ball, 
                                     game->player.position, 
                                     game->player.frameWidth * game->player.scale, 
                                     game->player.frameHeight * game->player.scale)) {
            game->player.hasBall = true;
            game->ball.isBeingCarried = true;
            game->ball.isVisible = false;
        }
        
       
        for (int i = 0; i < 3; i++) {
            Player *currentOpponent = &game->opponent[i];
            if (!currentOpponent->hasBall && currentOpponent->ballPickupCooldown <= 0 && 
                CheckBallPlayerCollision(&game->ball, 
                                         currentOpponent->position, 
                                         currentOpponent->frameWidth * currentOpponent->scale, 
                                         currentOpponent->frameHeight * currentOpponent->scale)) {
                currentOpponent->hasBall = true;
                game->ball.isBeingCarried = true;
                game->ball.isVisible = false;
                break;
            }
        }
    }
    
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
    Rectangle dest = { offsetX, 0, scaledW, scaledH };

    DrawTexturePro(game->campo, source, dest, (Vector2){0,0}, 0.0f, WHITE);

    DrawBall(&game->ball);
    DrawPlayer(&game->player);
    
    for (int i = 0; i < 3; i++) {
        DrawPlayer(&game->opponent[i]);
    }
    
    DrawText(TextFormat("TIME 1: %d | TIME 2: %d", game->score_team1, game->score_team2), 
             offsetX + 20, 20, 25, WHITE);

    if (!game->player.hasBall) {
        if (game->player.ballPickupCooldown > 0) {
            DrawText("AGUARDE PARA PEGAR A BOLA", 10, 50, 20, RED);
        } else {
            DrawText("CHEGUE PERTO DA BOLA PARA PEGÁ-LA", 10, 50, 20, WHITE);
        }
    } else {
        DrawText(TextFormat("FORÇA: %.0f%%", game->player.throwForce), 10, 70, 20, WHITE);
        DrawText("SEGURE Z PARA CARREGAR FORÇA E LANCE", 10, 50, 20, WHITE);
    }
    
    float midX = offsetX + scaledW / 2;
    DrawLine(midX, 0, midX, screenH, (Color){255, 255, 255, 100});
    
    exibirRanking(game, offsetX, screenH);

    EndDrawing();
}

void UnloadGame(Game *game) {
    UnloadTexture(game->campo);
    UnloadPlayer(&game->player);
    UnloadBall(&game->ball);
    ClearQueue(game->team1_wait_queue);
    ClearQueue(game->team2_wait_queue);
}