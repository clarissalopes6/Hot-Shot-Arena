#include "raylib.h"
#include "assets.h"

Assets load_assets(void) {
    Assets assets = {0};
    
    if (FileExists("assets/logo.png")) {
        assets.logo = LoadTexture("assets/logo.png");
    }
    
    if (FileExists("assets/player.png")) {
        assets.player = LoadTexture("assets/player.png");
    }
    
    if (FileExists("assets/ball.png")) {
        assets.ball = LoadTexture("assets/ball.png");
    }
    
    if (FileExists("assets/bola-dif.png")) {
        assets.ball_dif = LoadTexture("assets/bola-dif.png");
    }
    
    if (FileExists("assets/menu.png")) {
        assets.menu = LoadTexture("assets/menu.png");
    }
    
    if (FileExists("assets/instructions.png")) {
        assets.instructions = LoadTexture("assets/instructions.png");
    }

    if (FileExists("assets/court.png")) {
        assets.court = LoadTexture("assets/court.png");
    }
    
    if (FileExists("assets/lancador.png")) {
        assets.lancador = LoadTexture("assets/lancador.png");
    }
    
    return assets;
}

void unload_assets(Assets *assets) {
    if (assets->logo.id > 0) UnloadTexture(assets->logo);
    if (assets->player.id > 0) UnloadTexture(assets->player);
    if (assets->ball.id > 0) UnloadTexture(assets->ball);
    if (assets->ball_dif.id > 0) UnloadTexture(assets->ball_dif);
    if (assets->menu.id > 0) UnloadTexture(assets->menu);
    if (assets->instructions.id > 0) UnloadTexture(assets->instructions);
    if (assets->court.id > 0) UnloadTexture(assets->court);
    if (assets->lancador.id > 0) UnloadTexture(assets->lancador);
}
