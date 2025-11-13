#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

typedef struct {
    Texture2D texture;
    Vector2 position;
    float speed;
} Player;

Player InitPlayer();
void UpdatePlayer(Player *player, int screenWidth, int screenHeight);
void DrawPlayer(Player player);
void UnloadPlayer(Player player);

#endif
