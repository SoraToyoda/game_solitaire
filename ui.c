#include <ncurses.h>
#include <string.h>
#include <locale.h>
#include <time.h>
#include "ui.h"
#include "input.h"


int get_selection_cancel();
void clear_selection_cancel();
int get_cursor_col();
int get_selected_col();
Selection* get_selection();

void init_ui() {
    setlocale(LC_ALL, ""); // 日本語対応
    initscr();
    start_color();
    use_default_colors();
    init_pair(1, COLOR_RED, -1);
    init_pair(2, COLOR_WHITE, -1);
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
}

void end_ui() {
    endwin();
}

void draw_card(int y, int x, const Card* card) {
    int color = (card->suit == 'H' || card->suit == 'D') ? 1 : 2;
    attron(COLOR_PAIR(color));
    const char* suit_str = "?";
    switch(card->suit) {
        case 'S': suit_str = "\u2660"; break; // ♠
        case 'H': suit_str = "\u2665"; break; // ♥
        case 'D': suit_str = "\u2666"; break; // ♦
        case 'C': suit_str = "\u2663"; break; // ♣
    }
    if (card->value != 0) {
        mvprintw(y, x,   "+-------+");
        mvprintw(y+1, x, "| %2s %2s |", card_str(card), suit_str);
        mvprintw(y+2, x, "|       |");
        mvprintw(y+3, x, "| %2s %2s |", suit_str, card_str(card));
        mvprintw(y+4, x, "+-------+");
    } else {
        mvprintw(y, x,   "+-------+");
        mvprintw(y+1, x, "|       |");
        mvprintw(y+2, x, "|  --   |");
        mvprintw(y+3, x, "|       |");
        mvprintw(y+4, x, "+-------+");
    }
    attroff(COLOR_PAIR(color));
}

void draw_game(GameState* game) {
    clear();

    // 必要な画面サイズ
    const int required_rows = 35; // 行数
    const int required_cols = 85; // 列数

    // 現在の画面サイズを取得
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    // 画面サイズが不足している場合のエラーメッセージ
    if (rows < required_rows || cols < required_cols) {
        clear();
        const char* msg1 = "画面サイズが小さすぎます。";
        const char* msg2 = "必要なサイズ: 幅 85 x 高さ 35";
        const char* msg3 = "現在のサイズ: 幅 %d x 高さ %d";
        const char* msg4 = "ターミナルのサイズを変更して再実行してください。";

        int center_row = rows / 2;
        int center_col = (cols - strlen(msg1)) / 2;

        mvprintw(center_row - 2, center_col, "%s", msg1);
        mvprintw(center_row, center_col, msg2);
        mvprintw(center_row + 1, center_col, msg3, cols, rows);
        mvprintw(center_row + 3, center_col, "%s", msg4);
        refresh();
        return;
    }

    int cursor = get_cursor_col();
    Selection* sel = get_selection();

    // 説明の行を上部に表示
    mvprintw(0, 0, "←→: 移動 SPACE:選択/解除 s:山札をめくる d:場に出す f:場→組札 g:山札→組札 q:終了");
    mvprintw(1, 0, "複数選択: 場札でSPACE→[↑↓]で枚数選択→もう一度SPACEで確定→移動先でSPACEで移動");
    mvprintw(2, 0, "-----------------------------------------------------------------------------------");

    // 山札表示（draw_cardで表示、右側へずらす）
    mvprintw(3, 60, "[山札]");
    if (game->stock[game->stock_pos].value != 0) {
        draw_card(5, 60, &game->stock[game->stock_pos]);
    } else {
        mvprintw(5, 60, "+-------+");
        mvprintw(6, 60, "|       |");
        mvprintw(7, 60, "|  --   |");
        mvprintw(8, 60, "|       |");
        mvprintw(9, 60, "+-------+");
    }

    // 組札表示（draw_cardで表示、右側へずらす）
    for (int f = 0; f < 4; ++f) {
        int fx = 10 + f * 10; // 位置を調整
        mvprintw(3, fx, "組札%d", f+1);
        int top = -1;
        for (int i = 25; i >= 0; --i) {
            if (game->foundation[f][i].value != 0) { top = i; break; }
        }
        if (top >= 0) {
            draw_card(5, fx, &game->foundation[f][top]);
        } else {
            mvprintw(5, fx, "+-------+");
            mvprintw(6, fx, "|       |");
            mvprintw(7, fx, "|  --   |");
            mvprintw(8, fx, "|       |");
            mvprintw(9, fx, "+-------+");
        }
    }

    // 場札（メイン部分）
    for (int col = 0; col < 7; ++col) {
        int top = -1;
        for (int i = 25; i >= 0; --i) {
            if (game->tableau[col][i].value != 0) { top = i; break; }
        }
        for (int i = 0; i <= top; ++i) {
            int selected = 0;
            if (((sel->active == 1 || sel->active == 2) && sel->col == col && i >= sel->start && i <= sel->start + sel->count - 1)
                || (sel->active == 3 && col == cursor && i == 0)) // 組札→場札のときも強調
                selected = 1;
            if (selected) {
                if (sel->active == 1) attron(A_BOLD | A_REVERSE);
                else attron(A_REVERSE);
            }
            draw_card(11 + i * 2, 2 + col * 10, &game->tableau[col][i]); // 位置を調整
            if (selected) {
                if (sel->active == 1) attroff(A_BOLD | A_REVERSE);
                else attroff(A_REVERSE);
            }
        }
        // カーソル表示（場札）
        if (col == cursor) {
            if (get_selection_cancel()) {
                mvprintw(10, 2 + col * 10, "[解除]");
                clear_selection_cancel();
            } else if (sel->active == 1 && sel->col == col) {
                mvprintw(10, 2 + col * 10, "[選択]");
            } else if (sel->active == 2 && sel->col == col) {
                mvprintw(10, 2 + col * 10, "[確定]");
            } else if (sel->active == 3 && col == cursor) {
                mvprintw(10, 2 + col * 10, "[組札選択]");
            } else {
                mvprintw(10, 2 + col * 10, "   ↓   ");
            }
        }
    }
    // 組札にもカーソル・選択表示
    for (int f = 0; f < 4; ++f) {
        int fx = 10 + f * 10; // 位置を調整
        int idx = 7 + f;
        if (cursor == idx) {
            if (sel->active == 3 && sel->col == cursor) {
                mvprintw(10, fx, "[組札選択]");
            } else {
                mvprintw(10, fx, "   ↑   ");
            }
        }
    }
    refresh();
}


void draw_win_message() {
    // 画面中央に大きく表示
    int row, col;
    getmaxyx(stdscr, row, col);
    const char* msg1 = "おめでとうございます！";
    const char* msg2 = "プレイしていただきありがとうございます";
    // 2行分の高さを使い、幅も中央に
    attron(A_BOLD | A_REVERSE);
    mvprintw(row/2 - 2, (col - 36) / 2, " 　　　　　　　　　　　　　　　");
    mvprintw(row/2 - 1, (col - 36) / 2, "　　%s　　", msg1);
    mvprintw(row/2,     (col - 36) / 2, "　%s　", msg2);
    mvprintw(row/2 + 1, (col - 36) / 2, " 　　　　　　　　　　　　　　　");
    attroff(A_BOLD | A_REVERSE);
    refresh();
}

int wait_for_restart() {
    int ch;
    while ((ch = getch())) {
        if (ch == 'r') return 1;
        if (ch == 'q') return 0;
    }
    return 0;
}

// メニュー画面を描画し、選択肢を返す
int draw_menu() {
    clear();
    const char* title = "ソリティア";
    mvprintw(5, 30, "%s", title);
    mvprintw(10, 35, "1. START");
    mvprintw(12, 35, "2. HELP");
    mvprintw(14, 35, "3. EXIT");
    mvprintw(20, 30, "数字をを入力してください");
    refresh();

    int choice;
    while (1) {
        choice = getch();
        if (choice == '1') return 1;
        if (choice == '2') return 2;
        if (choice == '3' || choice == 'q') return 0;
    }
}