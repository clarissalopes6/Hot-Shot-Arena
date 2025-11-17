#include "list.h"
#include <stdlib.h>

BallList* ball_list_create(void) {
    BallList *list = (BallList*)malloc(sizeof(BallList));
    if (list) {
        list->head = NULL;
        list->tail = NULL;
        list->count = 0;
    }
    return list;
}

void ball_list_destroy(BallList *list) {
    if (!list) return;
    ball_list_clear(list);
    free(list);
}

Ball* ball_list_add(BallList *list) {
    if (!list) return NULL;
    
    Ball *new_ball = (Ball*)malloc(sizeof(Ball));
    if (!new_ball) return NULL;
    
    new_ball->x = 0;
    new_ball->y = 0;
    new_ball->vx = 0;
    new_ball->vy = 0;
    new_ball->speed = 0;
    new_ball->active = false;
    new_ball->is_different = false;
    new_ball->thrown_by_player = false;
    new_ball->max_travel = 0.0f;
    new_ball->travelled = 0.0f;
    new_ball->next = NULL;
    new_ball->prev = NULL;
    
    if (list->tail == NULL) {
        list->head = new_ball;
        list->tail = new_ball;
    } else {
        new_ball->prev = list->tail;
        list->tail->next = new_ball;
        list->tail = new_ball;
    }
    
    list->count++;
    return new_ball;
}

void ball_list_remove(BallList *list, Ball *ball) {
    if (!list || !ball) return;
    
    if (ball->prev) {
        ball->prev->next = ball->next;
    } else {
        list->head = ball->next;
    }
    
    if (ball->next) {
        ball->next->prev = ball->prev;
    } else {
        list->tail = ball->prev;
    }
    
    list->count--;
    free(ball);
}

void ball_list_clear(BallList *list) {
    if (!list) return;
    
    Ball *current = list->head;
    while (current) {
        Ball *next = current->next;
        free(current);
        current = next;
    }
    
    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
}
