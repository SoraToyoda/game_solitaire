#ifndef UI_H
#define UI_H
#include "game.h"
void init_ui();
void end_ui();
void draw_game(GameState* game); // 山札・組札も描画するよう拡張予定
void draw_win_message();
int wait_for_restart();
int draw_menu();
#endif
