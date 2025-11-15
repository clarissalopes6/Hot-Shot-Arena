#ifndef LIST_H
#define LIST_H

#include "player.h"
#include <stdbool.h>

typedef struct Node {
    Player data;
    struct Node *next;
} Node;

typedef struct Queue {
    Node *front;
    Node *rear;
    int count;
} Queue;

Queue *InitQueue();
void Enqueue(Queue *q, Player player);
Player Dequeue(Queue *q);
bool IsQueueEmpty(Queue *q);
void ClearQueue(Queue *q);

#endif