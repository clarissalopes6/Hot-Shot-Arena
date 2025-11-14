#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

typedef enum {
    STATE_IDLE,
    STATE_CATCH,
    STATE_PASS,
    STATE_THROW,
    STATE_RUN,
    STATE_DODGE,
    STATE_HIT
} PlayerState;

typedef struct Player {
    Texture2D texture;

    Rectangle sourceRec;
    Vector2 position;
    Vector2 velocity;

    int frameWidth;
    int frameHeight;

    int currentFrame;
    float frameTime;
    float frameSpeed;

    float speed;
    float scale;

    PlayerState state;
    int facing;    
} Player;

Player InitPlayer(void);
void UpdatePlayer(Player *player, float minX, float maxX, int screenH);
void DrawPlayer(Player *player);
void UnloadPlayer(Player *player);

#endif
