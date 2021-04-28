#pragma once
#include <vector>
#include "card.h"
#include <cstdlib>

class Deck
{
    public:
        Deck();
        Deck(Deck& ref);
        void addCard( unsigned int id, unsigned int offsetX, unsigned int offsetY, uint8_t topArrow, uint8_t rightArrow, uint8_t bottomArrow, uint8_t leftArrow);
        std::vector <Card*>  cards;

        unsigned int getRandomCardId();
        ~Deck();

};