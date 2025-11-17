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

static float pause_start_time = 0.0f;
static float paused_accum = 0.0f;

void init_game(GameState *state) {
    state->player.x = FIELD_LEFT + 50;
    state->player.y = FIELD_TOP + FIELD_HEIGHT/2 - PLAYER_SIZE/2;
    state->player.speed = 300.0f;
    state->player.lives = 3;
    
    state->tank.x = FIELD_RIGHT ; 
    state->tank.y = FIELD_TOP + FIELD_HEIGHT/2; 
    state->tank.shoot_timer = 0;
    state->tank.shoot_cooldown = 1.5f;
    state->tank.health = 30; 
    state->tank_max_health = state->tank.health;
    
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
    state->upgrade_generated = false;
    state->guarantee_next_gold = false;
    for (int i = 0; i < 3; i++) { state->upgrade_choice_type[i] = -1; state->upgrade_choice_tier[i] = TIER_SILVER; }
    state->extra_special_damage = 0;
    state->double_throw = false;
    state->resurrect_available = false;
    state->resurrect_used = false;
    state->paused_for_tank_upgrade = false;
    state->tank_upgrade_generated = false;
    state->tank_upgrade_choice = -1;
    state->tank_ball_speed_multiplier = 1.0f;
    state->player_charge_rate = CHARGE_RATE;
    
    if (state->balls) {
        ball_list_destroy(state->balls);
    }
    state->balls = ball_list_create();
    
    game_start_time = GetTime();
    pause_start_time = 0.0f;
    paused_accum = 0.0f;
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
        float base_speed = 400.0f * state->tank_ball_speed_multiplier;
        ball->vx = (dx / distance) * base_speed;
        ball->vy = (dy / distance) * base_speed;
    }
    ball->thrown_by_player = false;
    }


void update_game(GameState *state) {
    if (state->game_over) return;


    if (state->paused_for_tank_upgrade) {
        if (!state->tank_upgrade_generated) {
            state->tank_upgrade_choice = GetRandomValue(0,2); 
            state->tank_upgrade_generated = true;
        }

      
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            int choice = state->tank_upgrade_choice;
            if (choice == 0) {
                state->tank_ball_speed_multiplier *= 1.25f;
            } else if (choice == 1) {
                state->tank.health += 1;
                state->tank_max_health += 1;
            } else if (choice == 2) {
                state->player_charge_rate *= 1.5f;
            }
            state->paused_for_tank_upgrade = false;
            state->tank_upgrade_generated = false;
            state->tank_upgrade_choice = -1;
            if (pause_start_time > 0.0f) {
                paused_accum += GetTime() - pause_start_time;
                pause_start_time = 0.0f;
            }
        }
        return;
    }

    if (state->paused_for_upgrade) {
      
        if (!state->upgrade_generated) {
       
            int pool[3] = { UP_LIFE, UP_SPEED, UP_DMG };
         
            for (int i = 2; i > 0; i--) {
                int j = GetRandomValue(0, i);
                int tmp = pool[i]; pool[i] = pool[j]; pool[j] = tmp;
            }
            for (int k = 0; k < 3; k++) state->upgrade_choice_type[k] = pool[k];
       
            float base_heroic = 0.01f; 
            float base_epic = 0.05f; 
            float base_gold = 0.15f; 
            int thresh_heroic = (int)(base_heroic * 100.0f);
            int thresh_epic = thresh_heroic + (int)(base_epic * 100.0f);
            int thresh_gold = thresh_epic + (int)(base_gold * 100.0f);
            for (int k = 0; k < 3; k++) {
                int r = GetRandomValue(0, 99);
                if (r < thresh_heroic) state->upgrade_choice_tier[k] = TIER_HEROIC;
                else if (r < thresh_epic) state->upgrade_choice_tier[k] = TIER_EPIC;
                else if (r < thresh_gold) state->upgrade_choice_tier[k] = TIER_GOLD;
                else state->upgrade_choice_tier[k] = TIER_SILVER;
            }
          
            if (state->guarantee_next_gold) {
                int found = 0;
                for (int k = 0; k < 3; k++) if (state->upgrade_choice_tier[k] == TIER_GOLD || state->upgrade_choice_tier[k] == TIER_EPIC || state->upgrade_choice_tier[k] == TIER_HEROIC) found = 1;
                if (!found) {
                    int pick = GetRandomValue(0,2);
                    state->upgrade_choice_tier[pick] = TIER_GOLD;
                }
                state->guarantee_next_gold = false;
            }
            state->upgrade_generated = true;
        }

       
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
            for (int idx = 0; idx < 3; idx++) {
                Rectangle rc = (idx == 0) ? card0 : (idx == 1) ? card1 : card2;
                if (CheckCollisionPointRec(mp, rc)) {
                    int type = state->upgrade_choice_type[idx];
                    int tier = state->upgrade_choice_tier[idx];
               
                    if (type == UP_LIFE) {
                        if (tier == TIER_HEROIC) {
                        
                            state->resurrect_available = true;
                            state->resurrect_used = false;
                        } else if (tier == TIER_EPIC) {
                          
                            state->player.lives = 3; state->lives = 3;
                        } else if (tier == TIER_GOLD) {
                            state->player.lives += 2; state->lives += 2;
                        } else {
                            state->player.lives += 1; state->lives += 1;
                        }
                    } else if (type == UP_SPEED) {
                        if (tier == TIER_HEROIC) state->player.speed *= 1.50f;
                        else if (tier == TIER_EPIC) state->player.speed *= 1.30f;
                        else if (tier == TIER_GOLD) state->player.speed *= 1.25f;
                        else state->player.speed *= 1.10f;
                    } else if (type == UP_DMG) {
                        if (tier == TIER_HEROIC) { state->extra_special_damage += 3; state->double_throw = true; }
                        else if (tier == TIER_EPIC) state->extra_special_damage += 3;
                        else if (tier == TIER_GOLD) state->extra_special_damage += 2;
                        else state->extra_special_damage += 1;
                    }
                
                    state->paused_for_upgrade = false;
                    state->upgrade_generated = false;
                    if (pause_start_time > 0.0f) {
                        paused_accum += GetTime() - pause_start_time;
                        pause_start_time = 0.0f;
                    }
                    return;
                }
            }
        }
        return;
    }
    
    float elapsed = GetTime() - game_start_time;
    state->time_survived = elapsed;
   
    
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
            state->charge += state->player_charge_rate * GetFrameTime();
            if (state->charge >= 1.0f) {
                state->charge = 1.0f;
                state->charge_increasing = false;
            }
        } else {
            state->charge -= state->player_charge_rate * GetFrameTime();
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
           
            if (state->double_throw) {
                Ball *nb = ball_list_add(state->balls);
                if (nb) {
              
                    nb->x = b->x - b->vy * 0.05f;
                    nb->y = b->y + b->vx * 0.05f;
                    nb->vx = b->vx;
                    nb->vy = b->vy;
                    nb->active = true;
                    nb->thrown_by_player = true;
                    nb->is_different = b->is_different;
                    nb->max_travel = b->max_travel;
                    nb->travelled = b->travelled;
                }
            }
          
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
                     
                        if (state->resurrect_available && !state->resurrect_used) {
                            state->resurrect_used = true;
                            state->lives = 1;
                            state->player.lives = 1;
                          
                        } else {
                            state->game_over = true;
                        }
                    }
                  
                    if (!current->thrown_by_player && !state->paused_for_tank_upgrade && !state->game_over) {
                        state->paused_for_tank_upgrade = true;
                        state->tank_upgrade_generated = false;
                        if (pause_start_time <= 0.0f) pause_start_time = GetTime();
                    }
                }

            
                if (current->thrown_by_player && CheckCollisionRecs(tank_rect, ball_rect)) {
                    if (current->is_different) {
                  
                        state->tank.health -= (1 + state->extra_special_damage);
                      
                        if (state->tank.health <= 0) {
                  
                            state->tank.shoot_cooldown = 9999.0f;
                            state->victory = true;
                            state->game_over = true;
                           
                            if (state->player_name[0] != '\0') {
                                add_ranking_entry("ranking.txt", state->player_name, state->time_survived);
                            }
                        } else {
                        
                            state->paused_for_upgrade = true;
                            if (pause_start_time <= 0.0f) pause_start_time = GetTime();
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



    
    Ball *current = state->balls->head;
    while (current) {
            if (current->active) {
            if (current->is_different) {
            
                int cx = (int)(current->x + BALL_SIZE/2);
                int cy = (int)(current->y + BALL_SIZE/2);
                DrawCircle(cx, cy, BALL_SIZE/2, (Color){255,140,0,200}); 
         
                DrawCircle(cx, cy, (int)(BALL_SIZE * 0.7f), (Color){255,200,0,60});
            
                if (assets->ball.id > 0) {
                    Rectangle src_bd = { 0.0f, 0.0f, (float)assets->ball.width, (float)assets->ball.height };
                    float tex_scale = 0.85f;
                    Rectangle dest_bd = { current->x + (1.0f - tex_scale) * BALL_SIZE/2, current->y + (1.0f - tex_scale) * BALL_SIZE/2, (float)BALL_SIZE * tex_scale, (float)BALL_SIZE * tex_scale };
                    Vector2 origin_bd = { dest_bd.width/2, dest_bd.height/2 };
                    float rot = sinf(GetTime()*10.0f + current->x + current->y) * 12.0f;
                    BeginBlendMode(BLEND_ADDITIVE);
                    DrawTexturePro(assets->ball, src_bd, dest_bd, origin_bd, rot, (Color){255,220,180,200});
                    Rectangle dest_bd2 = dest_bd;
                    dest_bd2.x -= dest_bd2.width * 0.12f;
                    dest_bd2.y -= dest_bd2.height * 0.12f;
                    dest_bd2.width *= 1.24f;
                    dest_bd2.height *= 1.24f;
                    DrawTexturePro(assets->ball, src_bd, dest_bd2, (Vector2){dest_bd2.width/2, dest_bd2.height/2}, rot*0.6f, (Color){255,160,80,80});
                    EndBlendMode();
                }
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
        if (hb->is_different) {
            int cx = (int)(hb->x + BALL_SIZE/2);
            int cy = (int)(hb->y + BALL_SIZE/2);
            DrawCircle(cx, cy, BALL_SIZE/2, (Color){255,140,0,200});
            DrawCircle(cx, cy, (int)(BALL_SIZE * 0.7f), (Color){255,200,0,60});
            if (assets->ball.id > 0) {
                Rectangle src_h = { 0.0f, 0.0f, (float)assets->ball.width, (float)assets->ball.height };
                float tex_scale = 0.85f;
                Rectangle dest_h = { hb->x + (1.0f - tex_scale) * BALL_SIZE/2, hb->y + (1.0f - tex_scale) * BALL_SIZE/2, (float)BALL_SIZE * tex_scale, (float)BALL_SIZE * tex_scale };
                Vector2 origin_h = { dest_h.width/2, dest_h.height/2 };
                float rot = sinf(GetTime()*10.0f + hb->x + hb->y) * 12.0f;
                BeginBlendMode(BLEND_ADDITIVE);
                DrawTexturePro(assets->ball, src_h, dest_h, origin_h, rot, (Color){255,220,180,200});
                Rectangle dest_h2 = dest_h;
                dest_h2.x -= dest_h2.width * 0.12f;
                dest_h2.y -= dest_h2.height * 0.12f;
                dest_h2.width *= 1.24f;
                dest_h2.height *= 1.24f;
                DrawTexturePro(assets->ball, src_h, dest_h2, (Vector2){dest_h2.width/2, dest_h2.height/2}, rot*0.6f, (Color){255,160,80,80});
                EndBlendMode();
            }
        } else if (assets->ball.id > 0) {
            Rectangle src_h = { 0.0f, 0.0f, (float)assets->ball.width, (float)assets->ball.height };
            Rectangle dest_h = { hb->x, hb->y, (float)BALL_SIZE, (float)BALL_SIZE };
            Vector2 origin_h = { 0.0f, 0.0f };
            DrawTexturePro(assets->ball, src_h, dest_h, origin_h, 0.0f, WHITE);
        } else {
            DrawCircle((int)hb->x + BALL_SIZE/2, (int)hb->y + BALL_SIZE/2, BALL_SIZE/2, YELLOW);
        }
    }
    

    float display_time = time_remaining - paused_accum;
    if (pause_start_time > 0.0f) display_time -= (GetTime() - pause_start_time);
    DrawText(TextFormat("Tempo: %.1f", display_time), 20, 20, 40, WHITE);
    DrawText(TextFormat("Vidas: %d", state->lives), 20, 70, 40, RED);
  

  
    {
        int bar_w = 600;
        int bar_h = 20;
        int bx = SCREEN_WIDTH/2 - bar_w/2;
        int by = 16;
        DrawRectangle(bx - 2, by - 2, bar_w + 4, bar_h + 4, (Color){0,0,0,180});
        DrawRectangle(bx, by, bar_w, bar_h, (Color){60,60,60,220});
        float hp_ratio = (state->tank_max_health > 0) ? ((float)state->tank.health / (float)state->tank_max_health) : 0.0f;
        if (hp_ratio < 0.0f) hp_ratio = 0.0f;
        if (hp_ratio > 1.0f) hp_ratio = 1.0f;
        DrawRectangle(bx, by, (int)(bar_w * hp_ratio), bar_h, RED);
        DrawText(TextFormat("Lançador"), bx + 8, by - 2, 18, WHITE);
        DrawText(TextFormat("%d / %d", state->tank.health, state->tank_max_health), bx + bar_w - 80, by - 2, 16, WHITE);
    }

  
    if (state->paused_for_tank_upgrade) {
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0,0,0,160});
        const int CARD_W = 420;
        const int CARD_H = 180;
        int cx = SCREEN_WIDTH/2 - CARD_W/2;
        int cy = SCREEN_HEIGHT/2 - CARD_H/2;
        Rectangle rc = { (float)cx, (float)cy, (float)CARD_W, (float)CARD_H };
        DrawRectangleRec(rc, (Color){30,30,40,220});
        DrawRectangleLinesEx(rc, 4, (Color){200,80,10,255});
        int choice = state->tank_upgrade_choice;
        if (choice == 0) {
            DrawText("Lançador: Velocidade das bolas +25%", cx + 20, cy + 30, 20, WHITE);
            DrawText("As bolas do lançador ficaram mais rápidas.", cx + 20, cy + 70, 16, GRAY);
        } else if (choice == 1) {
            DrawText("Lançador: Vida +1", cx + 20, cy + 30, 22, WHITE);
            DrawText("O lançador ganhou 1 ponto de vida.", cx + 20, cy + 70, 16, GRAY);
        } else if (choice == 2) {
            DrawText("Lançador: Barra de lançamento mais rápida", cx + 20, cy + 30, 18, WHITE);
            DrawText("A barra de lançamento do jogador oscila mais rápido.", cx + 20, cy + 70, 14, GRAY);
        }
        DrawText("Clique para continuar", SCREEN_WIDTH/2 - 100, cy + CARD_H + 12, 16, YELLOW);
    }

  
    if (state->resurrect_available && !state->resurrect_used) {
        int tx = 240;
        int ty = 70;
        BeginBlendMode(BLEND_ADDITIVE);
        DrawCircle(tx, ty + 10, 14, (Color){255,90,0,200});
        DrawCircle(tx, ty + 10, 8, (Color){255,200,0,120});
        EndBlendMode();
        DrawText("R", tx - 6, ty - 2, 20, WHITE);
        DrawText("Ressurreição", tx + 20, ty - 6, 18, (Color){255,165,0,255});
    }

   
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

    
        Color silver = (Color){192,192,192,255};
        Color gold = (Color){255,215,0,255};
        for (int i = 0; i < 3; i++) {
            Rectangle rc = (i==0)?card0: (i==1)?card1:card2;
            int border_tier = state->upgrade_choice_tier[i];
            Color border = silver;
            if (border_tier == TIER_GOLD) border = gold;
            if (border_tier == TIER_EPIC) border = (Color){160,32,240,255};
     
            bool hovering = CheckCollisionPointRec(GetMousePosition(), rc);
            if (border_tier == TIER_HEROIC) {
              
                Color fire1 = (Color){255,90,0,200};
                Color fire2 = (Color){255,160,40,140};
                Color fire3 = (Color){255,220,120,100};
                if (hovering) {
                    DrawRectangleLinesEx(rc, 4, YELLOW);
                } else {
                    BeginBlendMode(BLEND_ADDITIVE);
                    DrawRectangleLinesEx(rc, 3, fire1);
                    Rectangle r2 = rc; r2.x -= 4; r2.y -= 4; r2.width += 8; r2.height += 8;
                    DrawRectangleLinesEx(r2, 2, fire2);
                    Rectangle r3 = rc; r3.x -= 8; r3.y -= 8; r3.width += 16; r3.height += 16;
                    DrawRectangleLinesEx(r3, 1, fire3);
                    EndBlendMode();
                }
            } else {
                Color hov = hovering ? YELLOW : border;
                DrawRectangleLinesEx(rc, 3, hov);
            }

          
            int type = state->upgrade_choice_type[i];
            int tier = state->upgrade_choice_tier[i];
            if (type == UP_LIFE) {
                if (tier == TIER_HEROIC) {
                    DrawText("Vida: Token de ressurreição (Heroico)", rc.x + 10, rc.y + 20, 16, WHITE);
                    DrawText("Concede um token que ressuscita uma vez com 1 vida.", rc.x + 10, rc.y + 60, 12, GRAY);
                } else if (tier == TIER_EPIC) {
                    DrawText("Vida: restaura tudo (Epico)", rc.x + 10, rc.y + 20, 18, WHITE);
                    DrawText("Restaura todas as vidas perdidas", rc.x + 10, rc.y + 60, 14, GRAY);
                } else if (tier == TIER_GOLD) {
                    DrawText("Vida +2 (Ouro)", rc.x + 20, rc.y + 30, 22, WHITE);
                    DrawText("Ganha 2 vidas!", rc.x + 20, rc.y + 70, 14, GRAY);
                } else {
                    DrawText("Vida +1 (Prata)", rc.x + 30, rc.y + 30, 20, WHITE);
                    DrawText("Ganha 1 vida", rc.x + 30, rc.y + 70, 14, GRAY);
                }
            } else if (type == UP_SPEED) {
                if (tier == TIER_HEROIC) {
                    DrawText("Velocidade +50% (Heroico)", rc.x + 10, rc.y + 30, 18, WHITE);
                    DrawText("Aumenta velocidade do jogador em 50% (forte)", rc.x + 10, rc.y + 70, 12, GRAY);
                } else if (tier == TIER_EPIC) {
                    DrawText("Velocidade +30% (Epico)", rc.x + 10, rc.y + 30, 18, WHITE);
                    DrawText("Aumenta velocidade em 30%", rc.x + 10, rc.y + 70, 14, GRAY);
                } else if (tier == TIER_GOLD) {
                    DrawText("Velocidade +25% (Ouro)", rc.x + 10, rc.y + 30, 18, WHITE);
                    DrawText("Aumenta velocidade em 25%", rc.x + 10, rc.y + 70, 14, GRAY);
                } else {
                    DrawText("Velocidade +10% (Prata)", rc.x + 20, rc.y + 30, 18, WHITE);
                    DrawText("Aumenta velocidade em 10%", rc.x + 20, rc.y + 70, 14, GRAY);
                }
            } else if (type == UP_DMG) {
                if (tier == TIER_HEROIC) {
                    DrawText("Dano +3 + Duplo (Heroico)", rc.x + 20, rc.y + 30, 20, WHITE);
                    DrawText("Bolas especiais +3 dano e atira duas bolas", rc.x + 10, rc.y + 70, 12, GRAY);
                } else if (tier == TIER_EPIC) {
                    DrawText("Dano +3 (Epico)", rc.x + 30, rc.y + 30, 22, WHITE);
                    DrawText("Bolas especiais +3 dano", rc.x + 20, rc.y + 70, 14, GRAY);
                } else if (tier == TIER_GOLD) {
                    DrawText("Dano +2 (Ouro)", rc.x + 30, rc.y + 30, 22, WHITE);
                    DrawText("Bolas especiais +2 dano", rc.x + 20, rc.y + 70, 14, GRAY);
                } else {
                    DrawText("Dano +1 (Prata)", rc.x + 30, rc.y + 30, 20, WHITE);
                    DrawText("Bolas especiais +1 dano", rc.x + 20, rc.y + 70, 14, GRAY);
                }
            }
        }

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
    float now = GetTime();
    float elapsed = now - game_start_time - paused_accum;
    if (pause_start_time > 0.0f) elapsed -= (now - pause_start_time);
    return elapsed;
}

bool is_game_over(GameState *state) {
    return state->game_over;
}
