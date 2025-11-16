#include "raylib.h"
#include "game.h"
#include <math.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define GAME_DURATION 60.0f
#define PLAYER_SIZE 32
#define BALL_SIZE 12

#define FIELD_LEFT 150
#define FIELD_RIGHT 1130
#define FIELD_TOP 100
#define FIELD_BOTTOM 620
#define FIELD_WIDTH (FIELD_RIGHT - FIELD_LEFT)
#define FIELD_HEIGHT (FIELD_BOTTOM - FIELD_TOP)
#define FIELD_CENTER (FIELD_LEFT + FIELD_WIDTH/2)

#define PLAYER_AREA_RIGHT (FIELD_LEFT + FIELD_WIDTH/2)
#define TANK_AREA_LEFT (FIELD_LEFT + FIELD_WIDTH/2)

static float game_start_time = 0;

void init_game(GameState *state) {
    state->player.x = FIELD_LEFT + 50;
    state->player.y = SCREEN_HEIGHT / 2;
    state->player.speed = 300.0f;
    state->player.lives = 3;
    
    state->tank.x = FIELD_RIGHT - 80;
    state->tank.y = SCREEN_HEIGHT / 2;
    state->tank.shoot_timer = 0;
    state->tank.shoot_cooldown = 1.5f;
    
    state->lives = 3;
    state->game_over = false;
    state->score = 0;
    state->time_survived = 0;
    
    if (state->balls) {
        ball_list_destroy(state->balls);
    }
    state->balls = ball_list_create();
    
    game_start_time = GetTime();
}

void shoot_ball(GameState *state) {
    Ball *ball = ball_list_add(state->balls);
    if (!ball) return;
    
    ball->x = state->tank.x;
    ball->y = state->tank.y;
    ball->active = true;
    
    float dx = state->player.x - ball->x;
    float dy = state->player.y - ball->y;
    float distance = sqrt(dx * dx + dy * dy);
    
    if (distance > 0) {
        ball->vx = (dx / distance) * 400;
        ball->vy = (dy / distance) * 400;
    }
}

void update_game(GameState *state) {
    if (state->game_over) return;
    
    float elapsed = GetTime() - game_start_time;
    state->time_survived = elapsed;
    state->score = (int)(elapsed * 10);
    
    if (IsKeyDown(KEY_RIGHT)) {
        state->player.x += state->player.speed * GetFrameTime();
    }
    if (IsKeyDown(KEY_LEFT)) {
        state->player.x -= state->player.speed * GetFrameTime();
    }
    if (IsKeyDown(KEY_DOWN)) {
        state->player.y += state->player.speed * GetFrameTime();
    }
    if (IsKeyDown(KEY_UP)) {
        state->player.y -= state->player.speed * GetFrameTime();
    }
    
    if (state->player.x < FIELD_LEFT) state->player.x = FIELD_LEFT;
    if (state->player.x > PLAYER_AREA_RIGHT - PLAYER_SIZE) state->player.x = PLAYER_AREA_RIGHT - PLAYER_SIZE;
    if (state->player.y < FIELD_TOP) state->player.y = FIELD_TOP;
    if (state->player.y > FIELD_BOTTOM - PLAYER_SIZE) state->player.y = FIELD_BOTTOM - PLAYER_SIZE;
    
    float difficulty = 1.0f + (elapsed / GAME_DURATION) * 1.5f;
    state->tank.shoot_cooldown = 1.5f / difficulty;
    state->tank.shoot_timer -= GetFrameTime();
    
    if (state->tank.shoot_timer <= 0) {
        shoot_ball(state);
        state->tank.shoot_timer = state->tank.shoot_cooldown;
    }
    
    Ball *current = state->balls->head;
    while (current) {
        Ball *next = current->next;
        
        if (current->active) {
            current->x += current->vx * GetFrameTime();
            current->y += current->vy * GetFrameTime();
            
            if (current->x < FIELD_LEFT || current->x > FIELD_RIGHT ||
                current->y < FIELD_TOP || current->y > FIELD_BOTTOM) {
                current->active = false;
            } else {
                Rectangle player_rect = {state->player.x, state->player.y, PLAYER_SIZE, PLAYER_SIZE};
                Rectangle ball_rect = {current->x, current->y, BALL_SIZE, BALL_SIZE};
                
                if (CheckCollisionRecs(player_rect, ball_rect)) {
                    current->active = false;
                    state->lives--;
                    state->player.lives--;
                    
                    if (state->lives <= 0) {
                        state->game_over = true;
                    }
                }
            }
        }
        
        current = next;
    }
}

void draw_field(void) {
    DrawRectangle(FIELD_LEFT, FIELD_TOP, FIELD_WIDTH, FIELD_HEIGHT, (Color){34, 139, 34, 255});
    
    DrawLineEx((Vector2){FIELD_LEFT, FIELD_TOP}, (Vector2){FIELD_LEFT, FIELD_BOTTOM}, 3, WHITE);
    DrawLineEx((Vector2){FIELD_RIGHT, FIELD_TOP}, (Vector2){FIELD_RIGHT, FIELD_BOTTOM}, 3, WHITE);
    DrawLineEx((Vector2){FIELD_LEFT, FIELD_TOP}, (Vector2){FIELD_RIGHT, FIELD_TOP}, 3, WHITE);
    DrawLineEx((Vector2){FIELD_LEFT, FIELD_BOTTOM}, (Vector2){FIELD_RIGHT, FIELD_BOTTOM}, 3, WHITE);
    
    DrawLineEx((Vector2){FIELD_CENTER, FIELD_TOP}, 
               (Vector2){FIELD_CENTER, FIELD_BOTTOM}, 3, WHITE);
    
    DrawCircleLines(FIELD_CENTER, FIELD_TOP + FIELD_HEIGHT/2, 40, WHITE);
    
    DrawRectangleLines(FIELD_LEFT + 50, FIELD_TOP + FIELD_HEIGHT/3, 80, FIELD_HEIGHT/3, WHITE);
    DrawRectangleLines(FIELD_RIGHT - 130, FIELD_TOP + FIELD_HEIGHT/3, 80, FIELD_HEIGHT/3, WHITE);
}

void draw_game(GameState *state, float time_remaining, Assets *assets) {
    BeginDrawing();
    ClearBackground((Color){34, 139, 34, 255});
    
    draw_field();
    
    if (assets->player.id > 0) {
        DrawTexture(assets->player, (int)state->player.x, (int)state->player.y, WHITE);
    } else {
        DrawRectangle((int)state->player.x, (int)state->player.y, PLAYER_SIZE, PLAYER_SIZE, BLUE);
    }
    
    if (assets->ball.id > 0) {
        DrawTexture(assets->ball, (int)state->tank.x - 20, (int)state->tank.y - 20, WHITE);
    } else {
        DrawRectangle((int)state->tank.x - 20, (int)state->tank.y - 20, 40, 40, RED);
    }
    
    Ball *current = state->balls->head;
    while (current) {
        if (current->active) {
            if (assets->ball.id > 0) {
                DrawTexture(assets->ball, (int)current->x, (int)current->y, WHITE);
            } else {
                DrawCircle((int)current->x + BALL_SIZE/2, (int)current->y + BALL_SIZE/2, BALL_SIZE/2, YELLOW);
            }
        }
        current = current->next;
    }
    
    DrawText(TextFormat("Tempo: %.1f", time_remaining), 20, 20, 40, WHITE);
    DrawText(TextFormat("Vidas: %d", state->lives), 20, 70, 40, RED);
    DrawText(TextFormat("Pontos: %d", state->score), 20, 120, 40, YELLOW);
    
    // Tela de game over
    if (state->game_over) {
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 200});
        DrawText("QUEIMADO!", SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2 - 50, 60, RED);
        DrawText("Voltando ao menu...", SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2 + 50, 30, WHITE);
    }
    
    EndDrawing();
}

float get_game_time_remaining(void) {
    float elapsed = GetTime() - game_start_time;
    float remaining = GAME_DURATION - elapsed;
    return remaining > 0 ? remaining : 0;
}

bool is_game_over(GameState *state) {
    return state->game_over || get_game_time_remaining() <= 0;
}
