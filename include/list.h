#ifndef LIST_H
#define LIST_H

#include <stdbool.h>

typedef struct Ball {
    float x;
    float y;
    float vx;
    float vy;
    float speed;
    bool active;
    bool is_different;
    bool thrown_by_player;
    float max_travel;
    float travelled;
    struct Ball *next;
    struct Ball *prev;
} Ball;

typedef struct {
    Ball *head;
    Ball *tail;
    int count;
} BallList;

BallList* ball_list_create(void);
void ball_list_destroy(BallList *list);
Ball* ball_list_add(BallList *list);
void ball_list_remove(BallList *list, Ball *ball);
void ball_list_clear(BallList *list);

#endif
