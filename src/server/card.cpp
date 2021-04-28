#include "card.h"

Card::Card(unsigned int id, unsigned int offsetX, unsigned int offsetY, uint8_t topArrow, uint8_t rightArrow, uint8_t bottomArrow, uint8_t leftArrow)
{
    this->id = id;
    this->offsetX = offsetX;
    this->offsetY = offsetY;
    this->topArrow = topArrow;
    this->rightArrow = rightArrow;
    this->bottomArrow = bottomArrow;
    this->leftArrow = leftArrow;
}

Card::Card(Card &card)
{
    this->id = card.id;
    this->offsetX = card.offsetX;
    this->offsetY = card.offsetY;
    this->topArrow = card.topArrow;
    this->rightArrow = card.rightArrow;
    this->bottomArrow = card.bottomArrow;
    this->leftArrow = card.leftArrow;
}