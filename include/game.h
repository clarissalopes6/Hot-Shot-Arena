#ifndef GAME_H
#define GAME_H

#include "player.h"

typedef struct {
    Texture2D campo;
    Player player;
} Game;

void InitGame(Game *game);
void UpdateGame(Game *game);
void DrawGame(Game *game);
void UnloadGame(Game *game);

#endif
