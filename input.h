#ifndef INPUT_H
#define INPUT_H
#include "game.h"
void handle_input(GameState* game);
int get_cursor_col();
int get_selected_col();
typedef struct {
    int col;
    int start;
    int count;
    int active;
} Selection;
Selection* get_selection();
#endif
