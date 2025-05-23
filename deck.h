#ifndef DECK_H
#define DECK_H

#define DECK_SIZE 52

typedef struct {
    int value; // 1 = A, ..., 13 = K
    char suit; // 'S', 'H', 'D', 'C'
} Card;

typedef struct {
    Card cards[DECK_SIZE];
} Deck;

void init_deck(Deck* deck);
void shuffle_deck(Deck* deck);
const char* card_str(const Card* card);

#endif