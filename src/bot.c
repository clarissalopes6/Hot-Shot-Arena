#include "bot.h"
#include "raylib.h"
#include "raymath.h"
#include <stdlib.h>
#include <math.h>

Player InitBot(Vector2 position, int teamID) {
    Player bot = InitPlayer();
    
    bot.position = position;
    bot.facing = -1; 
    
    return bot;
}

void UpdateBot(Player *bot, Ball *ball, float minX, float maxX, int screenH) {
    
    if (bot->state == STATE_HIT) {
         return;
    }
    if (bot->position.x <= minX) { 
        if (bot->hasBall) {
            bot->throwForce = 100.0f;
            bot->isCharging = true;
            bot->facing = 1; 
            if (GetRandomValue(1, 30) == 1) { 
                bot->isCharging = false;
            }
        }
        return;
    }
    bot->velocity = (Vector2){ 0, 0 };
    bot->state = STATE_IDLE;
    float playerWidth = bot->frameWidth * bot->scale;
    float playerHeight = bot->frameHeight * bot->scale;
    if (bot->hasBall) {
        bot->throwForce = 100.0f;
        bot->isCharging = true;
        if (GetRandomValue(1, 30) == 1) { 
            bot->isCharging = false;
        } 
    } 
    else {
        if (!ball->isBeingCarried && ball->z <= 0) {
            float distanceToBall = Vector2Length(Vector2Subtract(bot->position, ball->position));
            
            if (distanceToBall < 600.0f) {
                Vector2 direction = Vector2Normalize(Vector2Subtract(ball->position, bot->position));
                bot->velocity.x += direction.x * (bot->speed * 1.0f); 
                bot->velocity.y += direction.y * (bot->speed * 1.0f);
                bot->state = STATE_RUN;
                if (direction.x > 0) bot->facing = 1; else if (direction.x < 0) bot->facing = -1;
            }
        }
        if (bot->state == STATE_IDLE || bot->state == STATE_RUN) {
            if (GetRandomValue(0, 50) < 5) {
                int moveX = GetRandomValue(-1, 1);
                int moveY = GetRandomValue(-1, 1);
                
                bot->velocity.x += moveX * (bot->speed * 1.0f);
                bot->velocity.y += moveY * (bot->speed * 1.0f);
                bot->state = (moveX != 0 || moveY != 0) ? STATE_RUN : STATE_IDLE;
                
                if (moveX != 0) {
                    bot->facing = moveX;
                }
            }
        }
    }
    
    bot->velocity.x *= 0.85f;
    bot->velocity.y *= 0.85f;
    bot->position.x += bot->velocity.x;
    bot->position.y += bot->velocity.y;
    float midX = (minX + maxX) / 2;
    if (bot->position.x < midX) {
        bot->position.x = midX;
    }
    if (bot->position.x + playerWidth > maxX) {
        bot->position.x = maxX - playerWidth;
    }
    float maxQuadraY = (float)screenH;
    if (bot->position.y < 0) {
        bot->position.y = 0;
    }
    if (bot->position.y + playerHeight > maxQuadraY) {
        bot->position.y = maxQuadraY - playerHeight;
    }
}