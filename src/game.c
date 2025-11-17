#include "raylib.h"
#include "game.h"
#include <math.h>
#include <stdlib.h>
#include "ranking.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define GAME_DURATION 60.0f
#define PLAYER_SIZE 40
#define BALL_SIZE 20
#define TANK_SIZE 80

#define FIELD_LEFT 150
#define FIELD_RIGHT 1130
#define FIELD_TOP 100
#define FIELD_BOTTOM 620
#define FIELD_WIDTH (FIELD_RIGHT - FIELD_LEFT)
#define FIELD_HEIGHT (FIELD_BOTTOM - FIELD_TOP)
#define FIELD_CENTER (FIELD_LEFT + FIELD_WIDTH/2)

#define PLAYER_AREA_RIGHT (FIELD_LEFT + FIELD_WIDTH/2)
#define TANK_AREA_LEFT (FIELD_LEFT + FIELD_WIDTH/2)

#define PICKUP_RADIUS 40.0f
#define CHARGE_RATE 1.2f 
#define MAX_THROW_SPEED 900.0f

static float game_start_time = 0;

void init_game(GameState *state) {
    state->player.x = FIELD_LEFT + 50;
    state->player.y = FIELD_TOP + FIELD_HEIGHT/2 - PLAYER_SIZE/2;
    state->player.speed = 300.0f;
    state->player.lives = 3;
    
    state->tank.x = FIELD_RIGHT - 30; 
    state->tank.y = FIELD_TOP + FIELD_HEIGHT/2; 
    state->tank.shoot_timer = 0;
    state->tank.shoot_cooldown = 1.5f;
    state->tank.health = 10; 
    
    state->lives = 3;
    state->game_over = false;
    state->victory = false;
    state->score = 0;
    state->time_survived = 0;
    state->held_ball = NULL;
    state->charge = 0.0f;
    state->is_charging = false;
    state->charge_increasing = true;
    state->paused_for_upgrade = false;
    state->extra_special_damage = 0;
    
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
    ball->is_different = (GetRandomValue(0, 99) < 15);
    float dx = state->player.x - ball->x;
    float dy = state->player.y - ball->y;
    float distance = sqrt(dx * dx + dy * dy);
    
    if (distance > 0) {
        ball->vx = (dx / distance) * 400;
        ball->vy = (dy / distance) * 400;
    }
    ball->thrown_by_player = false;
    }


void update_game(GameState *state) {
    if (state->game_over) return;


    if (state->paused_for_upgrade) {
        const int CARD_W = 260;
        const int CARD_H = 140;
        const int GAP = 30;
        int total_w = CARD_W * 3 + GAP * 2;
        int start_x = SCREEN_WIDTH/2 - total_w/2;
        int y = SCREEN_HEIGHT/2 - CARD_H/2;

        Rectangle card0 = { (float)start_x, (float)y, (float)CARD_W, (float)CARD_H };
        Rectangle card1 = { (float)(start_x + CARD_W + GAP), (float)y, (float)CARD_W, (float)CARD_H };
        Rectangle card2 = { (float)(start_x + (CARD_W + GAP) * 2), (float)y, (float)CARD_W, (float)CARD_H };

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mp = GetMousePosition();
            if (CheckCollisionPointRec(mp, card0)) {
          
                state->player.lives += 1;
                state->lives += 1;
                state->paused_for_upgrade = false;
                return;
            } else if (CheckCollisionPointRec(mp, card1)) {
              
                state->player.speed *= 1.25f;
                state->paused_for_upgrade = false;
                return;
            } else if (CheckCollisionPointRec(mp, card2)) {
             
                state->extra_special_damage += 1;
                state->paused_for_upgrade = false;
                return;
            }
        }

       
        return;
    }
    
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

 
    if (!state->held_ball) {
        Ball *cur = state->balls->head;
        while (cur) {
            if (cur->active && cur->is_different) {
               
                float ball_cx = cur->x + BALL_SIZE/2.0f;
                float ball_cy = cur->y + BALL_SIZE/2.0f;
                float player_cx = state->player.x + PLAYER_SIZE/2.0f;
                float player_cy = state->player.y + PLAYER_SIZE/2.0f;
                float dx = ball_cx - player_cx;
                float dy = ball_cy - player_cy;
                float dist = sqrtf(dx*dx + dy*dy);
                if (dist <= PICKUP_RADIUS && IsKeyDown(KEY_SPACE)) {
                   
                    state->held_ball = cur;
                    cur->active = false; 
                    cur->vx = 0.0f;
                    cur->vy = 0.0f;
                    state->is_charging = true;
                    state->charge = 0.0f;
                    break;
                }
            }
            cur = cur->next;
        }
    }

  
    if (state->held_ball && state->is_charging) {
       
        if (state->charge_increasing) {
            state->charge += CHARGE_RATE * GetFrameTime();
            if (state->charge >= 1.0f) {
                state->charge = 1.0f;
                state->charge_increasing = false;
            }
        } else {
            state->charge -= CHARGE_RATE * GetFrameTime();
            if (state->charge <= 0.0f) {
                state->charge = 0.0f;
                state->charge_increasing = true;
            }
        }
        if (IsKeyPressed(KEY_Z)) {
        
            Ball *b = state->held_ball;
            float dx = state->tank.x - state->player.x;
            float dy = state->tank.y - state->player.y;
            float dist = sqrtf(dx*dx + dy*dy);
            float strength = state->charge;
            if (dist > 0) {
                b->vx = (dx / dist) * (MAX_THROW_SPEED * strength);
                b->vy = (dy / dist) * (MAX_THROW_SPEED * strength);
            } else {
                b->vx = MAX_THROW_SPEED * strength;
                b->vy = 0;
            }
           
              
            if (strength < 0.75f && dist > 0.0f) {
                float allowed = (strength / 0.75f) * dist; 
                b->max_travel = allowed;
                b->travelled = 0.0f;
            } else {
                b->max_travel = 0.0f; 
                b->travelled = 0.0f;
            }
            b->active = true;
            b->thrown_by_player = true; 
          
            state->held_ball = NULL;
            state->is_charging = false;
            state->charge = 0.0f;
            state->charge_increasing = true;
        }
    }
    
    Ball *current = state->balls->head;
    while (current) {
        Ball *next = current->next;
        
            if (current->active) {
            float dxm = current->vx * GetFrameTime();
            float dym = current->vy * GetFrameTime();
            current->x += dxm;
            current->y += dym;
      
            if (current->thrown_by_player && current->max_travel > 0.0f) {
                float moved = sqrtf(dxm*dxm + dym*dym);
                current->travelled += moved;
                if (current->travelled >= current->max_travel) {
                 
                    current->active = false;
                }
            }
            
            if (current->x < FIELD_LEFT || current->x > FIELD_RIGHT ||
                current->y < FIELD_TOP || current->y > FIELD_BOTTOM) {
                current->active = false;
            } else {
                Rectangle player_rect = {state->player.x, state->player.y, PLAYER_SIZE, PLAYER_SIZE};
                Rectangle ball_rect = {current->x, current->y, BALL_SIZE, BALL_SIZE};
                Rectangle tank_rect = { state->tank.x - (float)TANK_SIZE/2, state->tank.y - (float)TANK_SIZE/2, (float)TANK_SIZE, (float)TANK_SIZE };

                if (CheckCollisionRecs(player_rect, ball_rect)) {
                    current->active = false;
                    state->lives--;
                    state->player.lives--;
                    
                    if (state->lives <= 0) {
                        state->game_over = true;
                    }
                }

            
                if (current->thrown_by_player && CheckCollisionRecs(tank_rect, ball_rect)) {
                    if (current->is_different) {
                  
                        state->tank.health -= (1 + state->extra_special_damage);
                        state->score += 50; 
                        if (state->tank.health <= 0) {
                  
                            state->tank.shoot_cooldown = 9999.0f;
                            state->victory = true;
                            state->game_over = true;
                           
                            if (state->player_name[0] != '\0') {
                                add_ranking_entry("ranking.txt", state->player_name, state->time_survived);
                            }
                        } else {
                           
                            state->paused_for_upgrade = true;
                        }
                    }
                    current->active = false;
                }
            }
        }
        
        current = next;
    }

  
    if (state->held_ball) {
        state->held_ball->x = state->player.x + PLAYER_SIZE + 4; 
        state->held_ball->y = state->player.y + PLAYER_SIZE/2 - BALL_SIZE/2;
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
        Rectangle src = { 0.0f, 0.0f, (float)assets->player.width, (float)assets->player.height };
        Rectangle dest = { state->player.x, state->player.y, (float)PLAYER_SIZE, (float)PLAYER_SIZE };
        Vector2 origin = { 0.0f, 0.0f };
        DrawTexturePro(assets->player, src, dest, origin, 0.0f, WHITE);
    } else {
        DrawRectangle((int)state->player.x, (int)state->player.y, PLAYER_SIZE, PLAYER_SIZE, BLUE);
    }
    
    if (assets->lancador.id > 0) {
        Rectangle src_l = { 0.0f, 0.0f, (float)assets->lancador.width, (float)assets->lancador.height };
        Rectangle dest_l = { state->tank.x - (float)TANK_SIZE/2, state->tank.y - (float)TANK_SIZE/2, (float)TANK_SIZE, (float)TANK_SIZE };
        Vector2 origin_l = { 0.0f, 0.0f };
        DrawTexturePro(assets->lancador, src_l, dest_l, origin_l, 0.0f, WHITE);
    } else {
        DrawRectangle((int)state->tank.x - TANK_SIZE/2, (int)state->tank.y - TANK_SIZE/2, TANK_SIZE, TANK_SIZE, RED);
    }


    {
        float bar_w = 60.0f;
        float bar_h = 8.0f;
        float bx = state->tank.x - bar_w/2;
        float by = state->tank.y - (float)TANK_SIZE/2 - 14.0f;
        DrawRectangle((int)bx - 1, (int)by - 1, (int)bar_w + 2, (int)bar_h + 2, BLACK);
        DrawRectangle((int)bx, (int)by, (int)bar_w, (int)bar_h, GRAY);
        float hp_ratio = (state->tank.health > 0) ? ((float)state->tank.health / 6.0f) : 0.0f;
        DrawRectangle((int)bx, (int)by, (int)(bar_w * hp_ratio), (int)bar_h, RED);
    }
    
    Ball *current = state->balls->head;
    while (current) {
        if (current->active) {
            if (current->is_different && assets->ball_dif.id > 0) {
                Rectangle src_bd = { 0.0f, 0.0f, (float)assets->ball_dif.width, (float)assets->ball_dif.height };
                Rectangle dest_bd = { current->x, current->y, (float)BALL_SIZE, (float)BALL_SIZE };
                Vector2 origin_bd = { 0.0f, 0.0f };
                DrawTexturePro(assets->ball_dif, src_bd, dest_bd, origin_bd, 0.0f, WHITE);
            } else if (assets->ball.id > 0) {
                Rectangle src_b = { 0.0f, 0.0f, (float)assets->ball.width, (float)assets->ball.height };
                Rectangle dest_b = { current->x, current->y, (float)BALL_SIZE, (float)BALL_SIZE };
                Vector2 origin_b = { 0.0f, 0.0f };
                DrawTexturePro(assets->ball, src_b, dest_b, origin_b, 0.0f, WHITE);
            } else {
                DrawCircle((int)current->x + BALL_SIZE/2, (int)current->y + BALL_SIZE/2, BALL_SIZE/2, YELLOW);
            }
        }
        current = current->next;
    }

 
    if (state->held_ball) {
        Ball *hb = state->held_ball;
        if (hb->is_different && assets->ball_dif.id > 0) {
            Rectangle src_h = { 0.0f, 0.0f, (float)assets->ball_dif.width, (float)assets->ball_dif.height };
            Rectangle dest_h = { hb->x, hb->y, (float)BALL_SIZE, (float)BALL_SIZE };
            Vector2 origin_h = { 0.0f, 0.0f };
            DrawTexturePro(assets->ball_dif, src_h, dest_h, origin_h, 0.0f, WHITE);
        } else if (assets->ball.id > 0) {
            Rectangle src_h = { 0.0f, 0.0f, (float)assets->ball.width, (float)assets->ball.height };
            Rectangle dest_h = { hb->x, hb->y, (float)BALL_SIZE, (float)BALL_SIZE };
            Vector2 origin_h = { 0.0f, 0.0f };
            DrawTexturePro(assets->ball, src_h, dest_h, origin_h, 0.0f, WHITE);
        } else {
            DrawCircle((int)hb->x + BALL_SIZE/2, (int)hb->y + BALL_SIZE/2, BALL_SIZE/2, YELLOW);
        }
    }
    
    DrawText(TextFormat("Tempo: %.1f", time_remaining), 20, 20, 40, WHITE);
    DrawText(TextFormat("Vidas: %d", state->lives), 20, 70, 40, RED);
    DrawText(TextFormat("Pontos: %d", state->score), 20, 120, 40, YELLOW);

   
    if (state->is_charging && state->held_ball) {
        float bar_w = 80.0f;
        float bar_h = 10.0f;
        float bx = state->player.x + PLAYER_SIZE/2 - bar_w/2;
        float by = state->player.y - 20.0f;
        DrawRectangle((int)bx - 2, (int)by - 2, (int)bar_w + 4, (int)bar_h + 4, BLACK);
        DrawRectangle((int)bx, (int)by, (int)bar_w, (int)bar_h, GRAY);
        DrawRectangle((int)bx, (int)by, (int)(bar_w * state->charge), (int)bar_h, GREEN);
        if (state->charge >= 1.0f) {
            DrawText("MAX", (int)(bx + bar_w + 6), (int)by - 2, 12, YELLOW);
        }
    }
    
   
    if (state->paused_for_upgrade) {
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0,0,0,160});

        const int CARD_W = 260;
        const int CARD_H = 140;
        const int GAP = 30;
        int total_w = CARD_W * 3 + GAP * 2;
        int start_x = SCREEN_WIDTH/2 - total_w/2;
        int y = SCREEN_HEIGHT/2 - CARD_H/2;

        Rectangle card0 = { (float)start_x, (float)y, (float)CARD_W, (float)CARD_H };
        Rectangle card1 = { (float)(start_x + CARD_W + GAP), (float)y, (float)CARD_W, (float)CARD_H };
        Rectangle card2 = { (float)(start_x + (CARD_W + GAP) * 2), (float)y, (float)CARD_W, (float)CARD_H };

       
        DrawRectangleRec(card0, (Color){40,40,60,220});
        DrawRectangleRec(card1, (Color){40,40,60,220});
        DrawRectangleRec(card2, (Color){40,40,60,220});

    
        Color c0 = CheckCollisionPointRec(GetMousePosition(), card0) ? YELLOW : WHITE;
        Color c1 = CheckCollisionPointRec(GetMousePosition(), card1) ? YELLOW : WHITE;
        Color c2 = CheckCollisionPointRec(GetMousePosition(), card2) ? YELLOW : WHITE;
        DrawRectangleLinesEx(card0, 3, c0);
        DrawRectangleLinesEx(card1, 3, c1);
        DrawRectangleLinesEx(card2, 3, c2);

        DrawText("+1 Vida", card0.x + 30, card0.y + 40, 28, WHITE);
        DrawText("Ganha 1 vida extra", card0.x + 20, card0.y + 80, 14, GRAY);

        DrawText("+25% Velocidade", card1.x + 10, card1.y + 40, 24, WHITE);
        DrawText("Aumenta velocidade do jogador", card1.x + 10, card1.y + 80, 14, GRAY);

        DrawText("+1 Dano (bola dif)", card2.x + 10, card2.y + 40, 22, WHITE);
        DrawText("Especial causa +1 dano", card2.x + 20, card2.y + 80, 14, GRAY);

        DrawText("Clique em uma carta para selecionar", SCREEN_WIDTH/2 - 200, (int)(y + CARD_H + 20), 18, WHITE);
    }
    

    if (state->victory) {
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 200});
        DrawText("parabens voce botou quente!", SCREEN_WIDTH/2 - 220, SCREEN_HEIGHT/2 - 30, 40, GREEN);
        DrawText("Voltando ao menu...", SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2 + 30, 24, WHITE);
    } else if (state->game_over) {
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 200});
        DrawText("QUEIMADO!", SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2 - 50, 60, RED);
        DrawText("Voltando ao menu...", SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2 + 50, 30, WHITE);
    }
    
    EndDrawing();
}

float get_game_time_remaining(void) {
   
    float elapsed = GetTime() - game_start_time;
    return elapsed;
}

bool is_game_over(GameState *state) {
    return state->game_over;
}
