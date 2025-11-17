#ifndef GAME_H
#define GAME_H

#include "assets.h"
#include "list.h"

typedef enum { UP_LIFE = 0, UP_SPEED = 1, UP_DMG = 2, UP_TIER = 3 } UpgradeType;
typedef enum { TIER_SILVER = 0, TIER_GOLD = 1, TIER_EPIC = 2, TIER_HEROIC = 3 } TierType;

typedef struct {
    float x;
    float y;
    float speed;
    int lives;
} Player;

typedef struct {
    float x;
    float y;
    float shoot_timer;
    float shoot_cooldown;
    int health;
} Tank;

typedef struct {
    char name[50];
    int score;
    int lives_remaining;
    float time_survived;
} ScoreEntry;

typedef struct {
    Player player;
    Tank tank;
    BallList *balls;
    char player_name[64];
    Ball *held_ball;
    float charge;
    bool is_charging;
    bool charge_increasing;
    bool victory;
    int lives;
    bool game_over;
    int score;
    float time_survived;
  
    bool paused_for_upgrade; 
    bool upgrade_generated; 
    int upgrade_choice_type[3]; 
    int upgrade_choice_tier[3]; 
    bool guarantee_next_gold; 
    int extra_special_damage; 
  
    bool double_throw; 
    bool resurrect_available; 
    bool resurrect_used; 
    int tank_max_health; 

    bool paused_for_tank_upgrade; 
    bool tank_upgrade_generated; 
    int tank_upgrade_choice; 
   
    float tank_ball_speed_multiplier; 
    float player_charge_rate; 
} GameState;

void init_game(GameState *state);
void update_game(GameState *state);
void draw_game(GameState *state, float time_remaining, Assets *assets);
float get_game_time_remaining(void);
bool is_game_over(GameState *state);

#endif
