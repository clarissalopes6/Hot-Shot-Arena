#include "sort.h"
#include "player.h"
#include <stdbool.h>

void BubbleSortPlayers(Player *players, int count) {
    int i, j;
    Player temp;
    bool swapped;
    
    for (i = 0; i < count - 1; i++) {
        swapped = false;
        for (j = 0; j < count - 1 - i; j++) {
            if (players[j].score < players[j + 1].score) {
                temp = players[j];
                players[j] = players[j + 1];
                players[j + 1] = temp;
                swapped = true;
            }
        }
        if (swapped == false)
            break;
    }
}