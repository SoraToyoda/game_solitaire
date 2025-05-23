#include <ncurses.h>
#include <stdlib.h>
#include "input.h"
#include "game.h"

static Selection selection = {0, 0, 0, 0};

static int cursor_col = 0; // 0〜6:場札, 7〜10:組札

#define TABLEAU_COLS 7
#define FOUNDATION_COLS 4

// 選択解除要求フラグ
static int selection_cancel = 0;
int get_selection_cancel() { return selection_cancel; }
void clear_selection_cancel() { selection_cancel = 0; }

int get_cursor_col() {
    return cursor_col;
}

Selection* get_selection() {
    return &selection;
}

void handle_input(GameState* game) {
    int ch = getch();
    switch (ch) {
        case KEY_LEFT:
            if (cursor_col > 0) cursor_col--;
            break;
        case KEY_RIGHT:
            if (cursor_col < TABLEAU_COLS + FOUNDATION_COLS - 1) cursor_col++;
            break;
        case KEY_UP:
            if (selection.active == 1 && cursor_col == selection.col) {
                // 選択中に上矢印で選択枚数を増やす
                int col = cursor_col;
                int new_start = selection.start - 1;
                if (new_start >= 0) {
                    Card *upper = &game->tableau[col][new_start];
                    Card *lower = &game->tableau[col][new_start + 1];
                    if (upper->value == lower->value + 1 &&
                        ((upper->suit == 'H' || upper->suit == 'D') != (lower->suit == 'H' || lower->suit == 'D'))) {
                        selection.start = new_start;
                        selection.count++;
                    }
                }
            }
            break;
        case KEY_DOWN:
            if (selection.active == 1 && cursor_col == selection.col) {
                // 選択中に下矢印で選択枚数を減らす
                if (selection.count > 1) {
                    selection.start++;
                    selection.count--;
                }
            }
            break;
        case 's':
            // 山札めくり処理
            {
                int next = game->stock_pos + 1;
                // 次の有効なカードを探す
                while (next < 24 && game->stock[next].value == 0) next++;
                if (next >= 24) {
                    // 末尾まで行ったら最初の有効なカードに戻す
                    next = 0;
                    while (next < 24 && game->stock[next].value == 0) next++;
                }
                game->stock_pos = next;
            }
            break;
        case 'f':
            // 組札移動処理
            {
                int col = cursor_col;
                int idx = top_card_index(game, col);
                if (idx >= 0) {
                    Card* card = &game->tableau[col][idx];
                    for (int f = 0; f < 4; ++f) {
                        // 空 or 同じスートで1つ上の値
                        int top = -1;
                        for (int i = 12; i >= 0; --i) {
                            if (game->foundation[f][i].value != 0) { top = i; break; }
                        }
                        if ((top == -1 && card->value == 1) ||
                            (top >= 0 && game->foundation[f][top].suit == card->suit && game->foundation[f][top].value + 1 == card->value)) {
                            game->foundation[f][top+1] = *card;
                            card->value = 0;
                            card->suit = 0;
                            break;
                        }
                    }
                }
                break;
            }
        case ' ':
            if (!selection.active && cursor_col < TABLEAU_COLS) {
                // 場札: 選択開始
                int col = cursor_col;
                int top = top_card_index(game, col);
                if (top >= 0) {
                    selection.col = col;
                    selection.start = top;
                    selection.count = 1;
                    selection.active = 1; // 1:選択中
                    selection_cancel = 0;
                }
            } else if (!selection.active && cursor_col >= TABLEAU_COLS && cursor_col < TABLEAU_COLS + FOUNDATION_COLS) {
                // 組札: 選択開始（1枚のみ）
                int f = cursor_col - TABLEAU_COLS;
                int top = -1;
                for (int i = 12; i >= 0; --i) {
                    if (game->foundation[f][i].value != 0) { top = i; break; }
                }
                if (top >= 0) {
                    selection.col = cursor_col;
                    selection.start = top;
                    selection.count = 1;
                    selection.active = 3; // 3:組札選択
                    selection_cancel = 0;
                }
            } else if (selection.active == 1 && cursor_col == selection.col) {
                // 場札: 選択確定
                int ok = 1;
                int col = selection.col;
                for (int i = 0; i < selection.count - 1; ++i) {
                    Card *upper = &game->tableau[col][selection.start + i];
                    Card *lower = &game->tableau[col][selection.start + i + 1];
                    if (!(upper->value == lower->value + 1 &&
                        ((upper->suit == 'H' || upper->suit == 'D') != (lower->suit == 'H' || lower->suit == 'D')))) {
                        ok = 0;
                        break;
                    }
                }
                if (ok && selection.count > 0) {
                    selection.active = 2; // 2:確定
                    selection_cancel = 0;
                } else {
                    selection.active = 0;
                    selection_cancel = 1;
                }
            } else if (selection.active == 2 && cursor_col < TABLEAU_COLS) {
                // 場札: 複数選択確定後に移動
                int to_col = cursor_col;
                if (to_col != selection.col) {
                    int success = move_stack(game, selection.col, selection.start, selection.count, to_col);
                    if (success) {
                        selection.active = 0;
                        selection_cancel = 0;
                    } else {
                        selection.active = 0;
                        selection_cancel = 1;
                    }
                } else {
                    selection.active = 0;
                    selection_cancel = 1;
                }
            } else if (selection.active == 3 && cursor_col < TABLEAU_COLS) {
                // 組札選択→場札へ移動
                int from_f = selection.col - TABLEAU_COLS;
                int idx = selection.start;
                Card* card = &game->foundation[from_f][idx];
                int to_col = cursor_col;
                int to_idx = top_card_index(game, to_col);
                Card* to = (to_idx >= 0) ? &game->tableau[to_col][to_idx] : &(Card){0};
                if (can_move_card(card, to)) {
                    game->tableau[to_col][to_idx + 1] = *card;
                    card->value = 0;
                    card->suit = 0;
                    selection.active = 0;
                    selection_cancel = 0;
                } else {
                    selection.active = 0;
                    selection_cancel = 1;
                }
            } else {
                // 組札→組札や不正な場所で選択した場合も解除
                selection.active = 0;
                selection_cancel = 0;
            }
            break;
        case 'd': {
            // 山札のカードを場札に出す
            Card* card = &game->stock[game->stock_pos];
            if (card->value != 0) {
                int col = cursor_col;
                int idx = top_card_index(game, col);
                Card* to = &game->tableau[col][idx+1];
                if (can_move_card(card, (idx>=0)?&game->tableau[col][idx]:&(Card){0})) {
                    *to = *card;
                    card->value = 0;
                    card->suit = 0;
                }
            }
            break;
        }
        case 'g': {
            // 山札から組札にカードを出す
            Card* card = &game->stock[game->stock_pos];
            if (card->value != 0) {
                for (int f = 0; f < 4; ++f) {
                    int top = -1;
                    for (int i = 12; i >= 0; --i) {
                        if (game->foundation[f][i].value != 0) { top = i; break; }
                    }
                    if ((top == -1 && card->value == 1) ||
                        (top >= 0 && game->foundation[f][top].suit == card->suit && game->foundation[f][top].value + 1 == card->value)) {
                        game->foundation[f][top+1] = *card;
                        card->value = 0;
                        card->suit = 0;
                        break;
                    }
                }
            }
            break;
        }
        case 'q':
            endwin();
            exit(0);
    }
}
