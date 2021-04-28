#include "deck.h"

void Deck::addCard( unsigned int id, unsigned int offsetX, unsigned int offsetY, uint8_t topArrow, uint8_t rightArrow, uint8_t bottomArrow, uint8_t leftArrow)
{
    Card* card = new Card( id,  offsetX,  offsetY,  topArrow,  rightArrow,  bottomArrow, leftArrow);
    cards.push_back(card);
}

Deck::Deck(){}

Deck::Deck(Deck& ref)
{
    this->cards = ref.cards;
}

unsigned int Deck::getRandomCardId()
{
    int randomCardIndex = rand()%cards.size();
    unsigned int id = cards[randomCardIndex]->id;
    cards.erase(cards.begin() + randomCardIndex);
    return id;
}

Deck::~Deck(){
    for(auto i = cards.begin(); i<cards.end(); i++){
        delete *i;
    }
}