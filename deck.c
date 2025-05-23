#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "deck.h"
#include <stdio.h>

void init_deck(Deck* deck) {
    char suits[] = {'S', 'H', 'D', 'C'};
    int index = 0;
    for (int s = 0; s < 4; ++s) {
        for (int v = 1; v <= 13; ++v) {
            deck->cards[index].value = v;
            deck->cards[index].suit = suits[s];
            index++;
        }
    }
}

void shuffle_deck(Deck* deck) {
    srand(time(NULL));
    for (int i = DECK_SIZE - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        Card temp = deck->cards[i];
        deck->cards[i] = deck->cards[j];
        deck->cards[j] = temp;
    }
}

const char* card_str(const Card* card) {
    static char str[3];
    const char* faces[] = {"A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};
    snprintf(str, sizeof(str), "%s", faces[card->value - 1]);
    return str;
}