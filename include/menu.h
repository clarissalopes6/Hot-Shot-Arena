#ifndef MENU_H
#define MENU_H

#include "assets.h"

typedef enum {
    MENU_INITIAL,
    MENU_INSTRUCTIONS,
    MENU_EXIT
} MenuState;

MenuState draw_initial_menu(Assets *assets);
MenuState draw_instructions_menu(Assets *assets);

#endif
