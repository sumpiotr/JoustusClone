#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include "directionEnum.h"


class Card : public sf::Sprite
{
    public:
        Card(sf::Texture& texture, unsigned int id, int offsetX, int offsetY, int topArrow, int rightArrow, int bottomArrow, int leftArrow, bool isBlue);
        Card(const Card& ref);

        bool isBlue;

        void goToDeafultPosition();
        void moveCard(Direction direction, bool canGoOverBoard = false);

        void moveVisuallyCard(Direction direction);
        sf::Vector2f targetPosition;
        Direction moveDirection;

        int getArrow(Direction direction);
        static Direction getPushDirection(Card& card);
        static sf::Vector2f getDirectionVector(Direction direction);

        unsigned int id;

        static const int tilemapSize;
        static const float cardSize;
        int offsetX;
        int offsetY;

        sf::Vector2f deafultPosition;
        sf::Vector2f fieldPosition;

        int topArrow;
        int rightArrow;
        int bottomArrow;
        int leftArrow;

      

};


