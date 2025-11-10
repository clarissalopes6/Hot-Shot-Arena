#include "raylib.h"
#include <stdio.h>
#include <math.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define PLAYER_SPEED 5

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hot Shot Arena - Protótipo");
    SetTargetFPS(60);

    Texture2D campo = LoadTexture("Hot-Shot-Arena/assets/campo.png");
    Texture2D player = LoadTexture("Hot-Shot-Arena/assets/player.jpg");
    Texture2D bola = LoadTexture("Hot-Shot-Arena/assets/bola.png");

    float playerScale = 0.1f;
    float bolaScale = 0.05f;

    Vector2 playerPos = { (SCREEN_WIDTH - player.width * playerScale)/2.0f, (SCREEN_HEIGHT - player.height * playerScale)/2.0f };
    
    Vector2 bolaPos = { SCREEN_WIDTH/2.0f - (bola.width * bolaScale)/2.0f, SCREEN_HEIGHT/2.0f - (bola.height * bolaScale)/2.0f };
    Vector2 bolaVel = { 0, 0 };
    bool playerTemBola = false;
    float distanciaParaPegar = 50.0f;
    float tempoAposLancar = 0.0f;
    float delayParaPegar = 0.5f;
    
    float forcaLancamento = 0.0f;
    float forcaMinima = 200.0f;
    float forcaMaxima = 800.0f;
    float velocidadeCarga = 0.5f;
    bool carregandoLancamento = false;
    bool forcaSubindo = true;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        
        if (tempoAposLancar > 0) {
            tempoAposLancar -= dt;
        }
        
        if (IsKeyDown(KEY_RIGHT)) {
            playerPos.x += PLAYER_SPEED * dt * 60.0f;
        }
        if (IsKeyDown(KEY_LEFT)) {
            playerPos.x -= PLAYER_SPEED * dt * 60.0f;
        }
        if (IsKeyDown(KEY_UP)) {
            playerPos.y -= PLAYER_SPEED * dt * 60.0f;
        }
        if (IsKeyDown(KEY_DOWN)) {
            playerPos.y += PLAYER_SPEED * dt * 60.0f;
        }

        float pw = player.width * playerScale;
        float ph = player.height * playerScale;
        if (playerPos.x < 0) playerPos.x = 0;
        if (playerPos.y < 0) playerPos.y = 0;
        if (playerPos.x > SCREEN_WIDTH - pw) playerPos.x = SCREEN_WIDTH - pw;
        if (playerPos.y > SCREEN_HEIGHT - ph) playerPos.y = SCREEN_HEIGHT - ph;

        Vector2 playerCenter = { playerPos.x + pw/2, playerPos.y + ph/2 };
        
        if (!playerTemBola && tempoAposLancar <= 0) {
            float bw = bola.width * bolaScale;
            float bh = bola.height * bolaScale;
            Vector2 bolaCenter = { bolaPos.x + bw/2, bolaPos.y + bh/2 };
            float distancia = sqrt(pow(playerCenter.x - bolaCenter.x, 2) + pow(playerCenter.y - bolaCenter.y, 2));
            
            if (distancia < distanciaParaPegar) {
                playerTemBola = true;
                bolaVel = (Vector2){ 0, 0 };
            }
        }
        
        if (playerTemBola) {
            bolaPos.x = playerCenter.x - (bola.width * bolaScale)/2;
            bolaPos.y = playerCenter.y - (bola.height * bolaScale)/2;
            
            if (IsKeyDown(KEY_Z)) {
                carregandoLancamento = true;
                
                if (forcaSubindo) {
                    forcaLancamento += velocidadeCarga * dt;
                    if (forcaLancamento >= 1.0f) {
                        forcaLancamento = 1.0f;
                        forcaSubindo = false;
                    }
                } else {
                    forcaLancamento -= velocidadeCarga * dt;
                    if (forcaLancamento <= 0.0f) {
                        forcaLancamento = 0.0f;
                        forcaSubindo = true;
                    }
                }
            }
            
            if (IsKeyReleased(KEY_Z) && carregandoLancamento) {
                playerTemBola = false;
                tempoAposLancar = delayParaPegar;
                carregandoLancamento = false;
                
                float velocidade = forcaMinima + (forcaMaxima - forcaMinima) * forcaLancamento;
                
                bolaVel.x = velocidade;
                bolaVel.y = 0;
                
                forcaLancamento = 0.0f;
                forcaSubindo = true;
            }
        } else {
            bolaPos.x += bolaVel.x * dt;
            bolaPos.y += bolaVel.y * dt;
            
            bolaVel.x *= 0.99f;
            bolaVel.y *= 0.99f;
            
            if (fabs(bolaVel.x) < 5.0f) bolaVel.x = 0;
            if (fabs(bolaVel.y) < 5.0f) bolaVel.y = 0;
            
            float bw = bola.width * bolaScale;
            float bh = bola.height * bolaScale;
            float coeficienteRicochete = 0.7f;
            
            if (bolaPos.y < 0) {
                bolaPos.y = 0;
                bolaVel.y = -bolaVel.y * coeficienteRicochete;
            }
            if (bolaPos.y > SCREEN_HEIGHT - bh) {
                bolaPos.y = SCREEN_HEIGHT - bh;
                bolaVel.y = -bolaVel.y * coeficienteRicochete;
            }
            
            if (bolaPos.x < 0) {
                bolaPos.x = 0;
                bolaVel.x = -bolaVel.x * coeficienteRicochete;
            }
            if (bolaPos.x > SCREEN_WIDTH - bw) {
                bolaPos.x = SCREEN_WIDTH - bw;
                bolaVel.x = -bolaVel.x * coeficienteRicochete;
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTexture(campo, 0, 0, WHITE);
        DrawTextureEx(player, playerPos, 0.0f, playerScale, WHITE);
        DrawTextureEx(bola, bolaPos, 0.0f, bolaScale, WHITE);
        
        if (playerTemBola) {
            DrawText("Tem a bola! Segure Z para carregar força", 10, 10, 20, RED);
            
            if (carregandoLancamento) {
                int barraLargura = 200;
                int barraAltura = 30;
                int barraX = SCREEN_WIDTH/2 - barraLargura/2;
                int barraY = 50;
                
                DrawRectangle(barraX - 2, barraY - 2, barraLargura + 4, barraAltura + 4, BLACK);
                DrawRectangle(barraX, barraY, barraLargura, barraAltura, DARKGRAY);
                
                int larguraPreenchida = (int)(barraLargura * forcaLancamento);
                Color corBarra = GREEN;
                if (forcaLancamento > 0.66f) corBarra = RED;
                else if (forcaLancamento > 0.33f) corBarra = YELLOW;
                
                DrawRectangle(barraX, barraY, larguraPreenchida, barraAltura, corBarra);
                
                DrawText(TextFormat("Força: %d%%", (int)(forcaLancamento * 100)), barraX + 50, barraY + 5, 20, WHITE);
            }
        } else {
            DrawText("Chegue perto da bola para pegá-la", 10, 10, 20, BLUE);
        }
        
        EndDrawing();
    }

    UnloadTexture(campo);
    UnloadTexture(player);
    UnloadTexture(bola);
    CloseWindow();
    return 0;
}
