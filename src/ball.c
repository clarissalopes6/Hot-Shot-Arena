#include "ball.h"
#include "raylib.h"
#include <math.h>
#include <stdio.h>

Ball InitBall(void) {
    Ball ball = {0};
    
    ball.texture = LoadTexture("assets/ball.png");
    
    if (ball.texture.id <= 0) {
        ball.texture = LoadTexture("./assets/ball.png");
    }
    
    ball.radius = 16.0f;
    ball.scale = 1.5f;
    
    ball.position = (Vector2){400, 300};
    ball.velocity = (Vector2){0, 0};
    
    ball.isVisible = true;
    ball.isBeingCarried = false;
    
    return ball;
}

void UpdateBall(Ball *ball) {
    if (!ball->isBeingCarried) {
        ball->position.x += ball->velocity.x;
        ball->position.y += ball->velocity.y;
        
        ball->velocity.x *= 0.98f;
        ball->velocity.y *= 0.98f;
        
        if (fabs(ball->velocity.x) < 0.1f && fabs(ball->velocity.y) < 0.1f) {
            ball->velocity.x = 0;
            ball->velocity.y = 0;
        }
        
        float ballRadius = ball->radius * ball->scale;
        
        if (ball->position.x - ballRadius <= 0) {
            ball->position.x = ballRadius;
            ball->velocity.x = -ball->velocity.x * 0.8f;
        }
        
        if (ball->position.x + ballRadius >= GetScreenWidth()) {
            ball->position.x = GetScreenWidth() - ballRadius;
            ball->velocity.x = -ball->velocity.x * 0.8f;
        }
        
        if (ball->position.y - ballRadius <= 0) {
            ball->position.y = ballRadius;
            ball->velocity.y = -ball->velocity.y * 0.8f;
        }
        
        if (ball->position.y + ballRadius >= GetScreenHeight()) {
            ball->position.y = GetScreenHeight() - ballRadius;
            ball->velocity.y = -ball->velocity.y * 0.8f;
        }
    }
}

void DrawBall(Ball *ball) {
    if (ball->isVisible && !ball->isBeingCarried) {
        if (ball->texture.id > 0) {
            float size = ball->radius * 2 * ball->scale;
            Rectangle destRec = {
                ball->position.x - size/2,
                ball->position.y - size/2,
                size,
                size
            };
            Rectangle sourceRec = {0, 0, ball->texture.width, ball->texture.height};
            
            DrawTexturePro(ball->texture, sourceRec, destRec, (Vector2){0, 0}, 0, WHITE);
        } else {
            DrawCircle(ball->position.x, ball->position.y, ball->radius * ball->scale, ORANGE);
        }
    }
}

bool CheckBallPlayerCollision(Ball *ball, Vector2 playerPos, float playerWidth, float playerHeight) {
    if (ball->isBeingCarried || !ball->isVisible) return false;
    
    float closestX = fmax(playerPos.x, fmin(ball->position.x, playerPos.x + playerWidth));
    float closestY = fmax(playerPos.y, fmin(ball->position.y, playerPos.y + playerHeight));
    
    float distance = sqrt((ball->position.x - closestX) * (ball->position.x - closestX) +
                         (ball->position.y - closestY) * (ball->position.y - closestY));
    
    return distance <= ball->radius * ball->scale;
}

void ThrowBall(Ball *ball, Vector2 playerPos, int direction, float force) {
    if (!ball->isBeingCarried) return;
    
    ball->isBeingCarried = false;
    ball->isVisible = true;
    
    float playerWidth = 64;
    if (direction > 0) {
        ball->position.x = playerPos.x + playerWidth + 20;
    } else {
        ball->position.x = playerPos.x - 20;
    }
    ball->position.y = playerPos.y + 20;
    
    float throwForce = force / 100.0f * 25.0f;
    ball->velocity.x = direction * throwForce;
    ball->velocity.y = 0;
    
    printf("Throwing ball: direction=%d, force=%.2f, velocity.x=%.2f, playerPos=(%.2f,%.2f)\n", 
           direction, force, ball->velocity.x, playerPos.x, playerPos.y);
}

void UnloadBall(Ball *ball) {
    UnloadTexture(ball->texture);
}
