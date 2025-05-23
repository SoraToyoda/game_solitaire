#ifndef GAME_H
#define GAME_H
#include "deck.h"

typedef struct {
    Card tableau[7][13]; // 最大13枚まで
    Card stock[24];     // 山札（未使用カード）
    int stock_pos;      // 山札の現在位置
    Card foundation[4][13]; // 組札（4スート分）
} GameState;

void init_game(GameState* game, Deck* deck);
int check_win(GameState* game);
// 列間移動用
int top_card_index(GameState* game, int col);
int can_move_card(const Card* from, const Card* to);
int move_card(GameState* game, int from_col, int to_col);
int move_stack(GameState* game, int from_col, int start_idx, int count, int to_col);

#endif
