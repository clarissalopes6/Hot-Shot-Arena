#include "list.h"
#include <stdlib.h>
#include <stdio.h>

Queue *InitQueue() {
    Queue *q = (Queue *)malloc(sizeof(Queue));
    if (q == NULL) {
        return NULL;
    }
    q->front = NULL;
    q->rear = NULL;
    q->count = 0;
    return q;
}

bool IsQueueEmpty(Queue *q) {
    return (q->front == NULL);
}

void Enqueue(Queue *q, Player player) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    if (newNode == NULL) {
        return;
    }
    newNode->data = player;
    newNode->next = NULL;

    if (IsQueueEmpty(q)) {
        q->front = newNode;
        q->rear = newNode;
    } else {
        q->rear->next = newNode;
        q->rear = newNode;
    }
    q->count++;
}

Player Dequeue(Queue *q) {
    if (IsQueueEmpty(q)) {
        Player emptyPlayer = {0}; 
        return emptyPlayer; 
    }

    Node *temp = q->front;
    Player player = temp->data;

    q->front = q->front->next;
    
    if (q->front == NULL) {
        q->rear = NULL;
    }
    
    free(temp);
    q->count--;
    return player;
}

void ClearQueue(Queue *q) {
    Node *current = q->front;
    Node *next;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    q->front = NULL;
    q->rear = NULL;
    q->count = 0;
    free(q);
}