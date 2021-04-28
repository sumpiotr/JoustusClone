#include "card.h"
#include "board.h"


const float Card::cardSize = Board::boardFieldSize * 0.84f;
const int Card::tilemapSize = 510;

Card::Card(sf::Texture& texture, unsigned int id, int offsetX, int offsetY, int topArrow, int rightArrow, int bottomArrow, int leftArrow, bool isBlue ) : sf::Sprite(texture)
{
    setTextureRect(sf::IntRect(offsetX, offsetY, tilemapSize, tilemapSize));
    this->topArrow = topArrow;
    this->rightArrow = rightArrow;
    this->bottomArrow = bottomArrow;
    this->leftArrow = leftArrow;
    float targetSize = cardSize;
    this->id = id;
    this->isBlue = isBlue;
    setScale(sf::Vector2f(targetSize/tilemapSize, targetSize/tilemapSize));
}


Card::Card(const Card &ref) : sf::Sprite(*ref.getTexture())
{
    this->setTextureRect(ref.getTextureRect());
    this->setScale(ref.getScale());
    this->topArrow = ref.topArrow;
    this->rightArrow = ref.rightArrow;
    this->bottomArrow = ref.bottomArrow;
    this->leftArrow = ref.leftArrow;
    this->id = ref.id;
    this->isBlue = ref.isBlue;
}

void Card::goToDeafultPosition()
{
    setPosition(deafultPosition);
}

void Card::moveCard(Direction direction, bool canGoOverBoard)
{
    sf::Vector2f directionVector = getDirectionVector(direction);
    if((fieldPosition.x + Board::boardFieldSize * directionVector.x <= Board::centerCardPositionX + Board::boardFieldSize && 
        fieldPosition.x + Board::boardFieldSize * directionVector.x >= Board::centerCardPositionX - Board::boardFieldSize &&
        fieldPosition.y + Board::boardFieldSize * directionVector.y <= Board::centerCardPositionY + Board::boardFieldSize && 
        fieldPosition.y + Board::boardFieldSize * directionVector.y >= Board::centerCardPositionY - Board::boardFieldSize) || canGoOverBoard)
    {
        sf::Vector2f targetPosition;
        targetPosition.x = fieldPosition.x + Board::boardFieldSize * directionVector.x;
        targetPosition.y = fieldPosition.y + Board::boardFieldSize * directionVector.y;
        setPosition(targetPosition);
        fieldPosition = targetPosition;
    }
}

void Card::moveVisuallyCard(Direction direction)
{
    sf::Vector2f directionVector = getDirectionVector(direction);
    sf::Vector2f targetPosition;
    targetPosition.x = fieldPosition.x + (0.4 * Card::cardSize)*directionVector.x;
    targetPosition.y = fieldPosition.y + (0.4 * Card::cardSize)*directionVector.y;
    setPosition(targetPosition);
}





sf::Vector2f Card::getDirectionVector(Direction direction)
{
    switch(direction)
    {
        case up:
            return sf::Vector2f(0, -1);
        case left:
            return sf::Vector2f(-1, 0);
        case down:
            return sf::Vector2f(0, 1);
        case right:
            return sf::Vector2f(1, 0);
        default:
            return sf::Vector2f(0, 0);
    }

}

Direction Card::getPushDirection(Card& card)
{
    if(card.getPosition().y > card.fieldPosition.y)
    {
        return up;
    }
    else if(card.getPosition().y < card.fieldPosition.y)
    {
         return down;
    }
    else if(card.getPosition().x > card.fieldPosition.x)
    {
        return left;
    }
    else if(card.getPosition().x < card.fieldPosition.x)
    {
        return right;
    }
    
    return invalid;
}


int Card::getArrow(Direction direction)
{
    switch(direction)
    {
        case up:
            return topArrow;
        case left:
            return leftArrow;
        case down:
            return bottomArrow;
        case right:
            return rightArrow;
        default:
            return 100;
    }
}