#ifndef BALL_H
#define BALL_H

#include "raylib.h"

typedef struct {
    Texture2D texture;
    Vector2 position;
    Vector2 velocity;
    float radius;
    bool isVisible;
    bool isBeingCarried;
    float scale;
} Ball;

Ball InitBall(void);
void UpdateBall(Ball *ball);
void DrawBall(Ball *ball);
void UnloadBall(Ball *ball);
bool CheckBallPlayerCollision(Ball *ball, Vector2 playerPos, float playerWidth, float playerHeight);
void ThrowBall(Ball *ball, Vector2 playerPos, int direction, float force);

#endif
