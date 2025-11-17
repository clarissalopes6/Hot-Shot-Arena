#include "raylib.h"
#include "menu.h"
#include <string.h>
#include "ranking.h"
#include <stdio.h>
#include <stdlib.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define BUTTON_WIDTH 200
#define BUTTON_HEIGHT 60


static void draw_arena_background(Assets *assets) {
    (void)assets;
    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        float t = (float)i / (float)SCREEN_HEIGHT;
        Color c = (Color){ (unsigned char)(20 + 120 * (1.0f - t)), (unsigned char)(30 + 80 * (1.0f - t)), (unsigned char)(20 + 100 * t), 255 };
        DrawLine(0, i, SCREEN_WIDTH, i, c);
    }
    DrawRectangle(80, SCREEN_HEIGHT - 220, SCREEN_WIDTH - 160, 180, (Color){10,10,10,180});
    BeginBlendMode(BLEND_ADDITIVE);
    DrawCircle(140, SCREEN_HEIGHT - 140, 60, (Color){255,120,20,120});
    DrawCircle(SCREEN_WIDTH - 140, SCREEN_HEIGHT - 140, 60, (Color){255,80,10,120});
    EndBlendMode();
}

MenuState draw_initial_menu(Assets *assets) {
  
    int spacing = 20;
    int total_h = 3 * BUTTON_HEIGHT + 2 * spacing;
    int top_y = (SCREEN_HEIGHT - total_h) / 2 + 100; 
    Rectangle start_btn = {
        SCREEN_WIDTH/2 - BUTTON_WIDTH/2,
        top_y,
        BUTTON_WIDTH,
        BUTTON_HEIGHT
    };
    Rectangle instructions_btn = {
        SCREEN_WIDTH/2 - BUTTON_WIDTH/2,
        top_y + BUTTON_HEIGHT + spacing,
        BUTTON_WIDTH,
        BUTTON_HEIGHT
    };
    Rectangle ranking_btn = {
        SCREEN_WIDTH/2 - BUTTON_WIDTH/2,
        top_y + 2 * (BUTTON_HEIGHT + spacing),
        BUTTON_WIDTH,
        BUTTON_HEIGHT
    };
    
    BeginDrawing();
    ClearBackground(BLACK);
    
    if (assets->menu.id > 0) {
        DrawTexturePro(assets->menu, 
                       (Rectangle){0, 0, assets->menu.width, assets->menu.height},
                       (Rectangle){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT},
                       (Vector2){0, 0}, 0, WHITE);
    }
    
    Color start_color = CheckCollisionPointRec(GetMousePosition(), start_btn) ? YELLOW : WHITE;
    Color instr_color = CheckCollisionPointRec(GetMousePosition(), instructions_btn) ? YELLOW : WHITE;
    Color rank_color = CheckCollisionPointRec(GetMousePosition(), ranking_btn) ? YELLOW : WHITE;
    
    DrawRectangleRec(start_btn, (Color){0, 0, 0, 150});
    DrawRectangleLinesEx(start_btn, 3, start_color);
    DrawText("Começar Jogo", start_btn.x + 15, start_btn.y + 15, 20, start_color);
    
    DrawRectangleRec(instructions_btn, (Color){0, 0, 0, 150});
    DrawRectangleLinesEx(instructions_btn, 3, instr_color);
    DrawText("Instruções", instructions_btn.x + 35, instructions_btn.y + 15, 20, instr_color);

    DrawRectangleRec(ranking_btn, (Color){0, 0, 0, 150});
    DrawRectangleLinesEx(ranking_btn, 3, rank_color);
    DrawText("Ranking", ranking_btn.x + 60, ranking_btn.y + 15, 20, rank_color);
    
    EndDrawing();
    
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(GetMousePosition(), start_btn)) {
            return MENU_INITIAL;
        }
        if (CheckCollisionPointRec(GetMousePosition(), instructions_btn)) {
            return MENU_INSTRUCTIONS;
        }
        if (CheckCollisionPointRec(GetMousePosition(), ranking_btn)) {
            return MENU_RANKING;
        }
    }
    
    return -1;
}

MenuState draw_instructions_menu(Assets *assets) {
    Rectangle back_btn = {
        SCREEN_WIDTH/2 - BUTTON_WIDTH/2, 
        SCREEN_HEIGHT - 120, 
        BUTTON_WIDTH, 
        BUTTON_HEIGHT
    };
    
    BeginDrawing();
    ClearBackground(BLACK);
    
    
    if (assets->instructions.id > 0) {
        DrawTexturePro(assets->instructions,
                       (Rectangle){0, 0, assets->instructions.width, assets->instructions.height},
                       (Rectangle){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT},
                       (Vector2){0, 0}, 0, WHITE);
    } else {
       
        for (int i = 0; i < SCREEN_HEIGHT; i++) {
            float t = (float)i / (float)SCREEN_HEIGHT;
            Color c = (Color){ (unsigned char)(20 + 120 * (1.0f - t)), (unsigned char)(30 + 80 * (1.0f - t)), (unsigned char)(20 + 100 * t), 255 };
            DrawLine(0, i, SCREEN_WIDTH, i, c);
        }
       
        DrawRectangle(80, SCREEN_HEIGHT - 220, SCREEN_WIDTH - 160, 180, (Color){10,10,10,180});
        DrawText("Hot-Shot Arena", SCREEN_WIDTH/2 - 200, 60, 48, (Color){255,200,80,255});
        DrawText("Instruções", SCREEN_WIDTH/2 - 80, 120, 36, WHITE);

       
        BeginBlendMode(BLEND_ADDITIVE);
        DrawCircle(140, SCREEN_HEIGHT - 140, 60, (Color){255,120,20,120});
        DrawCircle(SCREEN_WIDTH - 140, SCREEN_HEIGHT - 140, 60, (Color){255,80,10,120});
        EndBlendMode();
    }

  
    DrawRectangle(100, 160, SCREEN_WIDTH - 220, SCREEN_HEIGHT - 340, (Color){0,0,0,140});
    DrawText("Regras e Objetivo:", 120, 180, 24, YELLOW);
    const char *rules[] = {
        "- Você controla o jogador na metade esquerda do campo.",
        "- O lançador inimigo fica na metade direita e atira bolas constantemente.",
        "- Pegue bolas especiais (laranja) com ESPAÇO. Pressione Z para carregar e lançar.",
        "- A duração do jogo é medida em tempo; o ranking usa seu tempo sobrevivido.",
        "- Ao acertar o lançador você ganha uma escolha de upgrade (pausa o jogo).",
        "- Upgrades têm raridades: Prata, Ouro, Épico, Heroico (raríssimo).",
        "- Quando o lançador te acerta ele pode receber um upgrade (o jogo pausa e mostra qual).",
        "- Você começa com 3 vidas; alguns upgrades restauram vidas ou dão token de ressurreição.",
        "- A barra de vida do lançador aparece no topo (estilo boss). Boa sorte!"
    };
    for (int r = 0; r < (int)(sizeof(rules)/sizeof(rules[0])); r++) {
        DrawText(rules[r], 120, 220 + r * 28, 20, WHITE);
    }
    
    Color back_color = CheckCollisionPointRec(GetMousePosition(), back_btn) ? YELLOW : WHITE;
    DrawRectangleRec(back_btn, (Color){0, 0, 0, 150});
    DrawRectangleLinesEx(back_btn, 3, back_color);
    DrawText("Voltar", back_btn.x + 55, back_btn.y + 15, 20, back_color);
    
    EndDrawing();
    
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(GetMousePosition(), back_btn)) {
            return MENU_INITIAL;
        }
    }
    
    return -1;
}

MenuState draw_ranking_menu(Assets *assets) {
    (void)assets;
    Rectangle back_btn = {
        SCREEN_WIDTH/2 - BUTTON_WIDTH/2,
        SCREEN_HEIGHT - 120,
        BUTTON_WIDTH,
        BUTTON_HEIGHT
    };

   
    RankEntry *entries = NULL;
    int count = load_ranking("ranking.txt", &entries);

    BeginDrawing();
    ClearBackground(BLACK);

    DrawText("Ranking - melhores tempos:", 120, 40, 24, WHITE);

    int show = count < 10 ? count : 10;
    for (int i = 0; i < show; i++) {
        char line[128];
        snprintf(line, sizeof(line), "%2d. %s - %.2fs", i+1, entries[i].name, entries[i].time);
        DrawText(line, 140, 100 + i*30, 20, YELLOW);
    }

    DrawRectangleRec(back_btn, (Color){0,0,0,150});
    Color back_color = CheckCollisionPointRec(GetMousePosition(), back_btn) ? YELLOW : WHITE;
    DrawRectangleLinesEx(back_btn, 3, back_color);
    DrawText("Voltar", back_btn.x + 55, back_btn.y + 15, 20, back_color);

    EndDrawing();

    if (entries) free(entries);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(GetMousePosition(), back_btn)) {
            return MENU_INITIAL;
        }
    }

    return -1;
}

MenuState draw_name_input(Assets *assets, char *out_name, int maxlen) {
    (void)assets; 
    static char namebuf[64] = {0};
    static int namelen = 0;
    if (maxlen < 2) return -1;


    int c = GetCharPressed();
    while (c > 0) {
       
        if ((c >= 32) && (c <= 125) && namelen < maxlen - 1) {
            namebuf[namelen++] = (char)c;
            namebuf[namelen] = '\0';
        }
        c = GetCharPressed();
    }
    if (IsKeyPressed(KEY_BACKSPACE) && namelen > 0) {
        namelen--;
        namebuf[namelen] = '\0';
    }

    BeginDrawing();

    if (assets->court.id > 0) {
        DrawTexturePro(assets->court,
                       (Rectangle){0,0,assets->court.width, assets->court.height},
                       (Rectangle){0,0,SCREEN_WIDTH, SCREEN_HEIGHT},
                       (Vector2){0,0}, 0.0f, WHITE);
    } else {
        draw_arena_background(assets);
    }
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0,0,0,120});
    DrawText("Digite seu nome (ENTER para confirmar):", 200, 200, 24, WHITE);
    DrawRectangle(200, 260, 600, 48, (Color){30,30,30,200});
    DrawRectangleLines(200, 260, 600, 48, WHITE);
    DrawText(namebuf, 210, 270, 28, YELLOW);
    EndDrawing();

    if (IsKeyPressed(KEY_ENTER) && namelen > 0) {
        strncpy(out_name, namebuf, maxlen - 1);
        out_name[maxlen - 1] = '\0';
      
        namelen = 0;
        namebuf[0] = '\0';
        return MENU_INITIAL;
    }

    return -1;
}
