#ifndef GAME_H
#define GAME_H

#include "player.h"
#include "ball.h"
#include "list.h"
#include "bot.h"
#include "sort.h"

typedef struct {
    Texture2D campo;
    Player player;
    Ball ball;
    
    Player opponent[3];
    
    Queue *team1_wait_queue;
    Queue *team2_wait_queue;
    
    int score_team1;
    int score_team2;
} Game;

void eliminarJogador(Player *player, Queue *waitQueue, Ball *ball);
void resgatarJogador(Player *player, Queue *waitQueue, Vector2 spawnPos);
void exibirRanking(Game *game, float offsetX, int screenH);

void InitGame(Game *game);
void UpdateGame(Game *game);
void DrawGame(Game *game);
void UnloadGame(Game *game);

#endif