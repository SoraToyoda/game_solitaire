#include <stdio.h>
#include "ui.h"
#include "deck.h"
#include "game.h"
#include "input.h"
#include "help.h"

int main(int argc, char *argv[]) {
    if (argc != 1) {
        fprintf(stderr, "エラー: ./solitaire で実行して下さい\n");
        return 1;
    }
    init_ui();
    while (1) {
        int choice = draw_menu();
        if (choice == 0) break;
        if (choice == 2) {
            show_help();
            continue;
        }

        Deck deck;
        init_deck(&deck);
        shuffle_deck(&deck);
        GameState game;
        init_game(&game, &deck);

        while (1) {
            draw_game(&game);
            if (check_win(&game)) {
                draw_win_message();
                if (wait_for_restart()) {
                    init_deck(&deck);
                    shuffle_deck(&deck);
                    init_game(&game, &deck);
                    continue;
                } else {
                    break;
                }
            }
            handle_input(&game);
        }
    }
    end_ui();
    return 0;
}
