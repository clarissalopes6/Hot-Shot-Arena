#include "player.h"

Player InitPlayer() {
    Player player;
    player.texture = LoadTexture("assets/player.png");
    player.position = (Vector2){ 400, 300 };
    player.speed = 5.0f;
    return player;
}

void UpdatePlayer(Player *player, int screenWidth, int screenHeight) {
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) player->position.x += player->speed;
    if (IsKeyDown(KEY_LEFT)  || IsKeyDown(KEY_A)) player->position.x -= player->speed;
    if (IsKeyDown(KEY_UP)    || IsKeyDown(KEY_W)) player->position.y -= player->speed;
    if (IsKeyDown(KEY_DOWN)  || IsKeyDown(KEY_S)) player->position.y += player->speed;

    if (player->position.x < 0) player->position.x = 0;
    if (player->position.y < 0) player->position.y = 0;
    if (player->position.x > screenWidth - player->texture.width)
        player->position.x = screenWidth - player->texture.width;
    if (player->position.y > screenHeight - player->texture.height)
        player->position.y = screenHeight - player->texture.height;
}

void DrawPlayer(Player player) {
    DrawTexture(player.texture, (int)player.position.x, (int)player.position.y, WHITE);
}

void UnloadPlayer(Player player) {
    UnloadTexture(player.texture);
}

