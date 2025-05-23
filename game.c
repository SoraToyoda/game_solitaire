#include "game.h"
#include <string.h>

void init_game(GameState* game, Deck* deck) {
    // 7列のソリティア初期配置
    int idx = 0;
    memset(game->tableau, 0, sizeof(game->tableau));
    for (int col = 0; col < 7; ++col) {
        for (int row = 0; row <= col; ++row) {
            game->tableau[col][row] = deck->cards[idx++];
        }
    }
    // 山札（残りカード）
    memset(game->stock, 0, sizeof(game->stock));
    for (int i = 0; idx < DECK_SIZE; ++i, ++idx) {
        game->stock[i] = deck->cards[idx];
    }
    game->stock_pos = 0;
    // 組札
    memset(game->foundation, 0, sizeof(game->foundation));
}

int check_win(GameState* game) {
    // 仮実装: すべての列が空なら勝利
    for (int col = 0; col < 7; ++col) {
        if (game->tableau[col][0].value != 0) return 0;
    }
    return 1;
}

// --- 列間移動ルールのための関数 ---

// 指定列の一番上のカードのインデックスを返す（なければ-1）
int top_card_index(GameState* game, int col) {
    for (int i = 12; i >= 0; --i) {
        if (game->tableau[col][i].value != 0) return i;
    }
    return -1;
}

// カードが移動可能か判定（簡易: 色違いで1小さい場合のみOK）
int can_move_card(const Card* from, const Card* to) {
    if (to->value == 0) return from->value == 13; // 空列にはKのみ可
    int color1 = (from->suit == 'H' || from->suit == 'D');
    int color2 = (to->suit == 'H' || to->suit == 'D');
    return (color1 != color2) && (from->value + 1 == to->value);
}

// カード移動処理
int move_card(GameState* game, int from_col, int to_col) {
    int from_idx = top_card_index(game, from_col);
    int to_idx = top_card_index(game, to_col);
    if (from_idx < 0) return 0;
    Card* from_card = &game->tableau[from_col][from_idx];
    Card dummy = {0};
    Card* to_card = (to_idx < 0) ? &dummy : &game->tableau[to_col][to_idx];
    if (!can_move_card(from_card, to_card)) return 0;
    // 移動
    if (to_idx < 0) {
        game->tableau[to_col][0] = *from_card;
    } else {
        game->tableau[to_col][to_idx+1] = *from_card;
    }
    from_card->value = 0;
    from_card->suit = 0;
    return 1;
}

// move_stackの実装を追加
int move_stack(GameState* game, int from_col, int start_idx, int count, int to_col) {
    int to_idx = top_card_index(game, to_col);
    Card* to_card = (to_idx < 0) ? &(Card){.value = 0, .suit = 0} : &game->tableau[to_col][to_idx];
    Card* first = &game->tableau[from_col][start_idx];

    // 先頭カードが移動可能かチェック（K->空列など）
    if (!can_move_card(first, to_card)) return 0;

    // 移動処理
    for (int i = 0; i < count; ++i) {
        Card* src = &game->tableau[from_col][start_idx + i];
        Card* dst = &game->tableau[to_col][to_idx + 1 + i];
        *dst = *src;
        src->value = 0;
        src->suit = 0;
    }
    return 1;
}
