#pragma once
#include <SFML/Graphics.hpp>
#include "card.h"


class Board: public sf::Sprite
{
    public:
        Board(sf::Texture &texture);
        ~Board();
        bool isFieldEmpty(sf::Vector2f fieldNumber);
        void setFieldContent(Card *card, sf::Vector2f fieldNumber);
        void reset();
        Card* getFieldContent(sf::Vector2f fieldNumber);
        sf::Vector2f getCardFieldNumber(sf::Vector2f cardFieldPosition);
        sf::Vector2f getCardPosition(sf::Vector2f fieldNumber);
        sf::Vector2f firstFieldPosition;
        static const float boardFieldSize;
        static float centerCardPositionX;
        static float centerCardPositionY;
        int fieldAmount = 5;
        float width;
        float height;
        Card **cardsOnBoard;
        std::vector <sf::Vector2f> gemsPositions;

};