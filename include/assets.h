#ifndef ASSETS_H
#define ASSETS_H

#include "raylib.h"

typedef struct {
    Texture2D logo;
    Texture2D player;
    Texture2D ball;
    Texture2D ball_dif;
    Texture2D menu;
    Texture2D instructions;
    Texture2D lancador;
} Assets;

Assets load_assets(void);
void unload_assets(Assets *assets);

#endif
