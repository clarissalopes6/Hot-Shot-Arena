#ifndef BOT_H
#define BOT_H

#include "player.h"
#include "ball.h"

void UpdateBot(Player *bot, Ball *ball, float minX, float maxX, int screenH);
Player InitBot(Vector2 position, int teamID);

#endif