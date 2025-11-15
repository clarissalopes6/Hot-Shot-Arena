#include "player.h"
#include "raylib.h"


Player InitPlayer(void) {
    Player player = {0};

    player.texture = LoadTexture("assets/player.png");
    
    if (player.texture.id <= 0) {
        player.texture = LoadTexture("./assets/player.png");
    }

    player.frameWidth  = player.texture.width  / 12;
    player.frameHeight = player.texture.height / 4;
    
    if (player.frameWidth <= 0) player.frameWidth = 32;
    if (player.frameHeight <= 0) player.frameHeight = 32;
    
    if (player.texture.width == 32 && player.texture.height == 32) {
        player.frameWidth = 32;
        player.frameHeight = 32;
    }

    player.sourceRec = (Rectangle){ 
        0, 
        0, 
        player.frameWidth, 
        player.frameHeight 
    };

    player.position = (Vector2){ 500, 600 };
    player.velocity = (Vector2){ 0, 0 };
    player.speed = 6.0f;

    player.scale = 2.0f; 

    player.state = STATE_IDLE;
    player.currentFrame = 0;
    player.frameTime = 0;
    player.frameSpeed = 0.08f;

    player.facing = 1;
    
    player.hasBall = false;
    player.throwForce = 0.0f;
    player.isCharging = false;
    player.ballPickupCooldown = 0.0f;
    player.score = 0;

    return player;
}


static int GetRowByState(PlayerState state) {
    switch (state) {
        case STATE_CATCH: return 0;
        case STATE_PASS:  return 0;
        case STATE_THROW: return 0;
        case STATE_RUN:   return 1;
        case STATE_DODGE: return 2;
        case STATE_IDLE:  return 0;
        case STATE_HIT:   return 3;
    }
    return 0;
}

static int GetFrameCount(PlayerState state) {
    switch (state) {
        case STATE_CATCH: return 3;
        case STATE_PASS:  return 3;
        case STATE_THROW: return 6;
        case STATE_RUN:   return 12;
        case STATE_DODGE: return 2;
        case STATE_IDLE:  return 1;
        case STATE_HIT:   return 8;
    }
    return 1;
}

void UpdatePlayer(Player *player, float minX, float maxX, int screenH) {
    player->velocity = (Vector2){ 0, 0 };
    player->state = STATE_IDLE;

    if (player->ballPickupCooldown > 0) {
        player->ballPickupCooldown -= GetFrameTime();
        if (player->ballPickupCooldown < 0) {
            player->ballPickupCooldown = 0;
        }
    }

    if (IsKeyDown(KEY_RIGHT)) { player->velocity.x += player->speed; player->state = STATE_RUN; player->facing = 1; }
    if (IsKeyDown(KEY_LEFT))  { player->velocity.x -= player->speed; player->state = STATE_RUN; player->facing = -1; }
    if (IsKeyDown(KEY_UP))    { player->velocity.y -= player->speed; player->state = STATE_RUN; }
    if (IsKeyDown(KEY_DOWN))  { player->velocity.y += player->speed; player->state = STATE_RUN; }

    if (IsKeyPressed(KEY_SPACE)) {
        player->state = STATE_DODGE;
    }

    if (player->hasBall) {
        if (IsKeyDown(KEY_Z)) {
            player->isCharging = true;
            player->state = STATE_THROW;
            
            static bool increasing = true;
            if (increasing) {
                player->throwForce += 150.0f * GetFrameTime();
                if (player->throwForce >= 100.0f) {
                    player->throwForce = 100.0f;
                    increasing = false;
                }
            } else {
                player->throwForce -= 150.0f * GetFrameTime();
                if (player->throwForce <= 0.0f) {
                    player->throwForce = 0.0f;
                    increasing = true;
                }
            }
        } else if (IsKeyReleased(KEY_Z) && player->isCharging) {
            player->isCharging = false;
           
        } else {
            player->isCharging = false;
            player->throwForce = 0.0f;
        }
    }
 
    if (IsKeyPressed(KEY_X)) player->state = STATE_PASS;
    if (IsKeyPressed(KEY_C)) player->state = STATE_THROW;

    player->position.x += player->velocity.x;
    player->position.y += player->velocity.y;

    if (player->position.x < minX) player->position.x = minX;
    
    if (player->position.x + player->frameWidth * player->scale > maxX)
        player->position.x = maxX - player->frameWidth * player->scale;

    float playerBottomY = player->position.y + player->frameHeight * player->scale;
    if (player->position.y < 0) player->position.y = 0;
    if (playerBottomY > screenH) 
        player->position.y = screenH - player->frameHeight * player->scale;
        
    int row = GetRowByState(player->state);
    int frameCount = GetFrameCount(player->state);

    player->frameTime += GetFrameTime();

    if (player->frameTime >= player->frameSpeed) {
        player->frameTime = 0;
        player->currentFrame++;

        if (player->currentFrame >= frameCount)
            player->currentFrame = 0;
    }

    float frameX = player->currentFrame * player->frameWidth;
    float frameY = row * player->frameHeight;
    
    player->sourceRec = (Rectangle){
        frameX,
        frameY,
        (float)player->frameWidth,
        (float)player->frameHeight
    };
}

void DrawPlayer(Player *player) {
    if (player->texture.id > 0) {
        Rectangle destRec = {
            player->position.x,
            player->position.y,
            player->frameWidth * player->scale * player->facing,
            player->frameHeight * player->scale
        };

        DrawTexturePro(
            player->texture,
            player->sourceRec,
            destRec,
            (Vector2){ 0, 0 },
            0,
            WHITE
        );
    } else {
        DrawRectangle(player->position.x, player->position.y, 64, 64, BLUE);
    }
    
    if (player->isCharging && player->hasBall) {
        float barWidth = 100.0f;
        float barHeight = 10.0f;
        float barX = player->position.x + (player->frameWidth * player->scale - barWidth) / 2;
        float barY = player->position.y - 20.0f;
        
        DrawRectangle(barX, barY, barWidth, barHeight, GRAY);
        
        float fillWidth = (player->throwForce / 100.0f) * barWidth;
        Color barColor = GREEN;
        if (player->throwForce > 66.0f) barColor = RED;
        else if (player->throwForce > 33.0f) barColor = YELLOW;
        
        DrawRectangle(barX, barY, fillWidth, barHeight, barColor);
        
        DrawRectangleLines(barX, barY, barWidth, barHeight, BLACK);
    }
    
    if (player->hasBall) {
        DrawCircle(player->position.x + player->frameWidth * player->scale / 2, 
                   player->position.y - 10, 5, ORANGE);
    }
}

void UnloadPlayer(Player *player) {
    UnloadTexture(player->texture);
}