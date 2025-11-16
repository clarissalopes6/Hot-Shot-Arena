#ifndef GAME_H
#define GAME_H

#include "assets.h"
#include "list.h"

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
    int lives;
    bool game_over;
    int score;
    float time_survived;
} GameState;

void init_game(GameState *state);
void update_game(GameState *state);
void draw_game(GameState *state, float time_remaining, Assets *assets);
float get_game_time_remaining(void);
bool is_game_over(GameState *state);

#endif
