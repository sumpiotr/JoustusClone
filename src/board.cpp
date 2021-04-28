#include "board.h"


const float Board::boardFieldSize = 80.0f;
float Board::centerCardPositionX = 0;
float Board::centerCardPositionY = 0;

Board::Board(sf::Texture& texture) : sf::Sprite(texture)
{
    sf::Vector2f scaleVector(boardFieldSize*5.0/texture.getSize().x, boardFieldSize*5.0/texture.getSize().y);
    setScale(scaleVector);
    width = boardFieldSize*fieldAmount;
    height = boardFieldSize*fieldAmount;

    firstFieldPosition.x = centerCardPositionX - 2 * boardFieldSize;
    firstFieldPosition.y = centerCardPositionY - 2 * boardFieldSize;

    cardsOnBoard = new Card*[25];
    for(int i = 0; i < 25; i++)
    {
        cardsOnBoard[i] = NULL;
    }
}

Board::~Board()
{
    delete cardsOnBoard;
}

bool Board::isFieldEmpty(sf::Vector2f fieldNumber)
{
    if(cardsOnBoard[(int)(fieldNumber.x + (fieldNumber.y*fieldAmount))] == NULL )return true;
    else return false;
}

void Board::setFieldContent(Card *card, sf::Vector2f fieldNumber)
{
    cardsOnBoard[(int)(fieldNumber.x + (fieldNumber.y*fieldAmount))] = card;
    if(cardsOnBoard[(int)(fieldNumber.x + (fieldNumber.y*fieldAmount))] == NULL ) std::cout << "h";
    card->deafultPosition = card->fieldPosition;
}

Card* Board::getFieldContent(sf::Vector2f fieldNumber)
{
    return cardsOnBoard[(int)(fieldNumber.x + (fieldNumber.y*fieldAmount))];
}

sf::Vector2f Board::getCardFieldNumber(sf::Vector2f cardFieldPosition)
{
    sf::Vector2f cardFieldNumber;
    cardFieldNumber.x = (cardFieldPosition.x - firstFieldPosition.x)/Board::boardFieldSize;
    cardFieldNumber.y = (cardFieldPosition.y - firstFieldPosition.y)/Board::boardFieldSize;
    return cardFieldNumber;
}

sf::Vector2f Board::getCardPosition(sf::Vector2f fieldNumber)
{
    sf::Vector2f targetPosition;
    targetPosition.x = firstFieldPosition.x + boardFieldSize * fieldNumber.x;
    targetPosition.y = firstFieldPosition.y + boardFieldSize * fieldNumber.y;
    return targetPosition;
}

void Board::reset()
{
    for(int i = 0; i < 25; i++)
    {
        cardsOnBoard[i] = NULL;
    }
    gemsPositions.clear();
}
