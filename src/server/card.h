#pragma once
#include <stdint.h>

class Card
{
    public:

        Card( unsigned int id, unsigned int offsetX, unsigned int offsetY, uint8_t topArrow, uint8_t rightArrow, uint8_t bottomArrow, uint8_t leftArrow);
        Card(Card &card);


        unsigned int id;
        unsigned int offsetX;
        unsigned int offsetY;

        uint8_t topArrow;
        uint8_t rightArrow;
        uint8_t bottomArrow;
        uint8_t leftArrow;
};

