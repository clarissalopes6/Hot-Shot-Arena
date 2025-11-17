#include "raylib.h"
#include "menu.h"
#include "game.h"
#include "assets.h"
#include "list.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

typedef enum {
    STATE_MENU,
    STATE_NAME_INPUT,
    STATE_RANKING,
    STATE_INSTRUCTIONS,
    STATE_GAME,
    STATE_EXIT
} AppState;

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hotshot");
    SetTargetFPS(60);
    
    Assets assets = load_assets();
    
    AppState state = STATE_MENU;
    GameState game_state = {0};
    game_state.balls = ball_list_create();
    float game_over_timer = 0;
    
    while (!WindowShouldClose() && state != STATE_EXIT) {
        switch (state) {
            case STATE_MENU: {
                MenuState menu_result = draw_initial_menu(&assets);
                if (menu_result == MENU_EXIT) {
                    state = STATE_EXIT;
                } else if (menu_result == MENU_INSTRUCTIONS) {
                    state = STATE_INSTRUCTIONS;
                } else if (menu_result == MENU_RANKING) {
                    state = STATE_RANKING;
                } else if (menu_result == MENU_INITIAL) {
                    state = STATE_NAME_INPUT;
                    game_over_timer = 0;
                }
                break;
            }
            case STATE_NAME_INPUT: {
                MenuState name_result = draw_name_input(&assets, game_state.player_name, sizeof(game_state.player_name));
                if (name_result == MENU_INITIAL) {
                    init_game(&game_state);
                    state = STATE_GAME;
                    game_over_timer = 0;
                }
                break;
            }
            case STATE_RANKING: {
                MenuState rank_result = draw_ranking_menu(&assets);
                if (rank_result == MENU_INITIAL) {
                    state = STATE_MENU;
                }
                break;
            }
            case STATE_INSTRUCTIONS: {
                MenuState menu_result = draw_instructions_menu(&assets);
                if (menu_result == MENU_INITIAL) {
                    state = STATE_MENU;
                }
                break;
            }
            case STATE_GAME: {
                update_game(&game_state);
                draw_game(&game_state, get_game_time_remaining(), &assets);
                
                if (is_game_over(&game_state)) {
                    game_over_timer += GetFrameTime();
                    if (game_over_timer > 3.0f) {
                        state = STATE_MENU;
                    }
                }
                break;
            }
            default:
                break;
        }
    }
    
    ball_list_destroy(game_state.balls);
    unload_assets(&assets);
    CloseWindow();
    return 0;
}
