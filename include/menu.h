#ifndef MENU_H
#define MENU_H

#include "assets.h"

typedef enum {
    MENU_INITIAL,
    MENU_INSTRUCTIONS,
    MENU_RANKING,
    MENU_EXIT
} MenuState;

MenuState draw_initial_menu(Assets *assets);
MenuState draw_instructions_menu(Assets *assets);
MenuState draw_name_input(Assets *assets, char *out_name, int maxlen);
MenuState draw_ranking_menu(Assets *assets);

#endif
