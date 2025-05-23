#include "ui.h"
#include "deck.h"
#include "game.h"
#include "input.h"

int main() {
    init_ui();
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

    end_ui();
    return 0;
}