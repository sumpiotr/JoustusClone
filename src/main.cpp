#include <SFML/Graphics.hpp>
#include <iostream>
#include "board.h"
#include "card.h"
#include <vector>
#include <cstdlib>
#include "onlineManager.h"
#include <thread>
#include <SFML/System/Time.hpp>
#include <cstdio>

#define c(x) std::cout << x << std::endl; fflush(stdout);

sf::RenderWindow *window;

Board *board;

Card *focusCard;
Card *selectedCard = NULL;

sf::RectangleShape focusBorder;



sf::Texture *blueCardsTilemap = new sf::Texture();
sf::Texture *redCardsTilemap = new sf::Texture();

sf::Texture *gemTexture = new sf::Texture();
std::vector <sf::Sprite*> gems;

sf::Vector2f bluePlayerHandCardPositions[3];
sf::Vector2f redPlayerHandCardPositions[3];

Card *bluePlayerHandCards[3];
Card *redPlayerHandCards[3];
uint8_t activeCardIndex;

bool pushingCards = false;

std::vector<Card*> pushedCards;

bool canPlayAnimation = true;
bool playingAnimation = false;
int animationState = 0;

std::vector<Card*> animatedCards;

sf::Clock animationTimer;

sf::Text gameEndText;
bool isEnd = false;

std::map<unsigned int, Card*> cards;

bool isBlue;
bool isMyTurn = false;

OnlineManager* onlineManager;

enum Winner
{
    bluePlayer,
    redPlayer,
    draw,
};

void close();
bool isGameEnd();
void endGame();
bool canPush(Direction direction, Card& pushedCard, int arrow);
void restartGame();

//My change
void addCardToHand(bool isBlueCard, Card* card)
{
    if(isBlueCard){
        int indexToPlace = -1;
        while(bluePlayerHandCards[++indexToPlace] != NULL);
        bluePlayerHandCards[indexToPlace] = card;
        bluePlayerHandCards[indexToPlace]->setPosition(bluePlayerHandCardPositions[indexToPlace]);
        bluePlayerHandCards[indexToPlace]->deafultPosition = bluePlayerHandCardPositions[indexToPlace];
        activeCardIndex = 0;
    }
    else
    {
        int indexToPlace = -1;
        while(redPlayerHandCards[++indexToPlace] != NULL);
        redPlayerHandCards[indexToPlace] = card;
        redPlayerHandCards[indexToPlace]->setPosition(redPlayerHandCardPositions[indexToPlace]);
        redPlayerHandCards[indexToPlace]->deafultPosition = redPlayerHandCardPositions[indexToPlace];
        std::cout << indexToPlace;
        activeCardIndex = 0;
    }
}

void addCardToPlayer(bool isMine, Card* card){
    if(isMine)
    {
        addCardToHand(isBlue, card);
    }
    else
    {
        addCardToHand(!isBlue, card);
    }
}


void setPushCards(Card& pushedCard, Direction direction);

void receiveFromServer(char* hostName, uint16_t port, char* key)
{
    std::cout << "In online thread" << std::endl;
    onlineManager = new OnlineManager(hostName, port, key);
    bool running = true;
    while(running){
        sf::Packet event = onlineManager->listen();
        int type;
        event >> type;

        switch(type)
        {
            case EVENT_EXIT:{
                    c("EVENT_EXIT");
                    running = false;
                    if(window->isOpen()){
                        window->close();
                    }
                    break;
                }
            case EVENT_ADD_PLAYER_CARD:{
                    c("EVENT_ADD_PLAYER_CARD");
                    unsigned int id;
                    bool isMine;
                    event >> id;
                    event >> isMine;
                    addCardToPlayer(isMine, cards[id]);
                    break;
                }
            case EVENT_DEFINE_CARD:{
                    c("EVENT_DEFINE_CARD");
                    bool isMine;
                    unsigned int id;
                    unsigned int offsetX;
                    unsigned int offsetY;

                    event >> isMine;
                    event >> id;
                    event >> offsetX;
                    event >> offsetY;

                    uint8_t topArrow;
                    uint8_t rightArrow;
                    uint8_t bottomArrow;
                    uint8_t leftArrow;

                    event >> topArrow;
                    event >> rightArrow;
                    event >> bottomArrow;
                    event >> leftArrow;

                    Card* newCard;

                    bool isBlueTileset = isMine ? isBlue : !isBlue;

                    if(isMine)
                    {
                        if(isBlue)newCard = new Card(*blueCardsTilemap, id, offsetX, offsetY, topArrow, rightArrow, bottomArrow, leftArrow, true);
                        else newCard = new Card(*redCardsTilemap, id, offsetX, offsetY, topArrow, rightArrow, bottomArrow, leftArrow, false);
                    }
                    else
                    {
                        if(!isBlue)newCard = new Card(*blueCardsTilemap, id, offsetX, offsetY, topArrow, rightArrow, bottomArrow, leftArrow, true);
                        else newCard = new Card(*redCardsTilemap, id, offsetX, offsetY, topArrow, rightArrow, bottomArrow, leftArrow, false);
                    }

                    cards[id] = newCard;
                    cards[id]->setPosition(sf::Vector2f(10000, 10000));
                    break;
                }
            case EVENT_MOVE_CARD:{
                c("EVENT_MOVE_CARD");
                std::cout << "Tak" << std::endl;
                unsigned int id;
                event >> id;
                int fieldNumberX;
                int fieldNumberY;
                event >> fieldNumberX;
                event >> fieldNumberY;
                int directionInt;
                event >> directionInt;
                Direction direction = (Direction)directionInt;
                sf::Vector2f fieldNumber(fieldNumberX, fieldNumberY);
                setPushCards(*board->getFieldContent(fieldNumber),  direction);
                pushingCards = true;
                if(isBlue)
                {
                    for(int i = 0; i < sizeof(redPlayerHandCards); i++)
                    {
                        if(redPlayerHandCards[i] == cards[id])redPlayerHandCards[i] = NULL;
                    }
                }
                else
                {
                    for(int i = 0; i < sizeof(bluePlayerHandCards); i++)
                    {
                        if(bluePlayerHandCards[i] == cards[id])bluePlayerHandCards[i] = NULL;
                    }
                }
                cards[id]->fieldPosition = board->getCardPosition(fieldNumber);
                board->setFieldContent(cards[id], fieldNumber);
                cards[id]->goToDeafultPosition();
                if(isGameEnd())
                {
                    onlineManager->broadcastGameEnd();
                    endGame();
                }
                break;
            }
            case EVENT_SET_COLOR:{
                c("EVENT_SET_COLOR");
                event >> isBlue;
                break;
            }
            case EVENT_TURN_START:{
                c("EVENT_TURN_START");
                if(!isEnd)
                {
                    focusCard = isBlue ? bluePlayerHandCards[0] : redPlayerHandCards[0];
                    isMyTurn = true;
                }
                break;
            }
            case EVENT_SET_GEM:{
                c("EVENT_SET_GEM");
                int positionX;
                int positionY;
                event >> positionX;
                event >> positionY;
                sf::Vector2f fieldNumber(positionX, positionY);


                sf::Vector2f position = board->getCardPosition(fieldNumber);
                board->gemsPositions.push_back(position);
                sf::Sprite* gem = new sf::Sprite(*gemTexture);
                float targetScaleX = (Card::cardSize)/gemTexture->getSize().x;
                float targetScaleY = (Card::cardSize)/gemTexture->getSize().y;
                sf::Vector2f visualPosition;
                visualPosition.x = window->getSize().x/2 + ((gemTexture->getSize().x * targetScaleX) /2) + positionX*Board::boardFieldSize;
                visualPosition.y = window->getSize().y/2 + ((gemTexture->getSize().y * targetScaleY) /2) + positionY*Board::boardFieldSize;
                gem->setPosition(position);
                gem->setScale(targetScaleX, targetScaleY);
                gems.push_back(gem);
                break;
            }
            case EVENT_END_GAME:{
                endGame();
                break;
            }
            case EVENT_RESTART_GAME:{
                restartGame();
                onlineManager->broadcasNewGame();
                break;
            }
        }
    }
}

void setTextures()
{
    //Set blue deck
    blueCardsTilemap->loadFromFile("assets/joustus deck.png");
    redCardsTilemap->loadFromFile("assets/red deck joustus.png");
    gemTexture->loadFromFile("assets/Joustus-Gems.png");

}

Winner GetWinner()
{
    uint8_t bluePlayerGems = 0;
    uint8_t redPlayerGems = 0;
    for(sf::Vector2f position : board->gemsPositions)
    {
        sf::Vector2f fieldNumber = board->getCardFieldNumber(position);
        if(!board->isFieldEmpty(fieldNumber))
        {
            Card card = *board->getFieldContent(fieldNumber);
            card.isBlue ? bluePlayerGems++ : redPlayerGems++;
        }
    }
    if(bluePlayerGems > redPlayerGems)return bluePlayer;
    else if(redPlayerGems > bluePlayerGems) return redPlayer;
    else return  draw;
}

void endGame()
{
    Winner winner = GetWinner();
    if(winner == bluePlayer)
    {
        gameEndText.setFillColor(sf::Color::Blue);
        gameEndText.setOutlineColor(sf::Color::Blue);
        gameEndText.setString("Blue Win!");
    }
    else if(winner == redPlayer)
    {
        gameEndText.setFillColor(sf::Color::Red);
        gameEndText.setOutlineColor(sf::Color::Red);
        gameEndText.setString("Red Win!");
    }
    else
    {
        gameEndText.setString("Draw!");
    }
    isEnd = true;
}


bool isGameEnd()
{
    bool isGemField = false;
    bool isEmptyField = false;
    for(int x = 1; x < 4; x++)
    {
        for(int y = 1; y < 4; y++)
        {
            for(sf::Vector2f position : board->gemsPositions)
            {
                if(position == board->getCardPosition(sf::Vector2f(x, y)))
                {
                    isGemField = true;
                }
            }

            if(isGemField)
            {
                isGemField = false;
                if(board->isFieldEmpty(sf::Vector2f(x, y)))isEmptyField = true;
                continue;
            }
            if(board->isFieldEmpty(sf::Vector2f(x, y)))return false;
        }
    }
    if(!isEmptyField)return true;

    for(Card *card: isBlue ? bluePlayerHandCards : redPlayerHandCards)
    {
        for(int x = 1; x < 4; x++)
        {
            for(int y = 1; y < 4; y++)
            {
                Card& cardToCheck = *board->getFieldContent(sf::Vector2f(x, y));
                if(canPush(up, cardToCheck, cardToCheck.topArrow))
                {
                    return false;
                }
                if(canPush(left, cardToCheck, cardToCheck.leftArrow))
                {
                    return false;
                }
                if(canPush(down, cardToCheck, cardToCheck.bottomArrow))
                {
                    return false;
                }
                if(canPush(right, cardToCheck, cardToCheck.rightArrow))
                {
                    return false;
                }
            }
        }
    }

    return true;

}

void placeSelectedCardOnBoard(Direction direction)
{

    if(selectedCard == NULL)return;
    sf::Vector2f cardFieldNumber = board->getCardFieldNumber(selectedCard->fieldPosition);
    board->setFieldContent(selectedCard, cardFieldNumber);

    onlineManager->broadcastTurnEnd(selectedCard->id,  cardFieldNumber, direction);

    if(isBlue)bluePlayerHandCards[activeCardIndex] = NULL;
    else redPlayerHandCards[activeCardIndex] = NULL;

    selectedCard->deafultPosition = selectedCard->fieldPosition;
    selectedCard->setPosition(selectedCard->fieldPosition);
    selectedCard = NULL;
    focusCard = bluePlayerHandCards[activeCardIndex];
    isMyTurn = false;
}

void returnCardToHand()
{
    if(selectedCard != NULL)
    {
        focusCard = selectedCard;
        selectedCard = NULL;
        focusCard->goToDeafultPosition();
    }
}

Direction getOppositeDirection(Direction direction)
{
    switch(direction)
    {
        case up:
            return down;
        case left:
            return right;
        case down:
            return up;
        case right:
            return left;
        default:
            return invalid;
    }
}



bool canPush(Direction direction, Card& pushedCard, int arrow)
{
    if(arrow == 0)return false;
    if(arrow <= pushedCard.getArrow(getOppositeDirection(direction)))return false;
    sf::Vector2f directionVector = Card::getDirectionVector(direction); 
    sf::Vector2f nextCardPosition = pushedCard.fieldPosition;
    nextCardPosition.x += Board::boardFieldSize * directionVector.x;
    nextCardPosition.y += Board::boardFieldSize * directionVector.y;
    sf::Vector2f nextCardFieldNumber = board->getCardFieldNumber(nextCardPosition);
    if(nextCardPosition.x   > Board::centerCardPositionX + 2 * Board::boardFieldSize ||
        nextCardPosition.x  < Board::centerCardPositionX - 2 * Board::boardFieldSize ||
        nextCardPosition.y  > Board::centerCardPositionY + 2 * Board::boardFieldSize || 
        nextCardPosition.y  < Board::centerCardPositionY - 2 * Board::boardFieldSize)
    {
        return false;
    }


    if(board->isFieldEmpty(nextCardFieldNumber)) return true;
    else
    {
        return canPush(direction, *board->getFieldContent(nextCardFieldNumber), arrow);
    }

}

void setPushCards(Card& pushedCard, Direction direction)
{
    if(direction == invalid)return;
    sf::Vector2f directionVector = Card::getDirectionVector(direction);
    sf::Vector2f nextCardPosition = pushedCard.fieldPosition;
    nextCardPosition.x += Board::boardFieldSize * directionVector.x;
    nextCardPosition.y += Board::boardFieldSize * directionVector.y;
    sf::Vector2f nextCardFieldNumber = board->getCardFieldNumber(nextCardPosition);
    pushedCard.targetPosition = nextCardPosition;
    pushedCard.moveDirection = direction;
    pushedCards.push_back(&pushedCard);
    pushedCard.fieldPosition = nextCardPosition;
    pushedCard.deafultPosition = pushedCard.fieldPosition;
    if(!board->isFieldEmpty(nextCardFieldNumber))
    {
        Card& nextCard = *board->getFieldContent(nextCardFieldNumber);
        board->setFieldContent(&pushedCard, nextCardFieldNumber);
        setPushCards(nextCard, direction);
    }
    else
    {
        board->setFieldContent(&pushedCard, nextCardFieldNumber);
    }

}

void finishMove(){
    Direction direction = pushedCards[0]->moveDirection;
    for(Card *card : pushedCards){
        card->moveDirection = invalid;
        card->targetPosition = sf::Vector2f(0, 0);
    }
    pushedCards.clear();
    placeSelectedCardOnBoard(direction);
    pushingCards = false;
}


void pushCards(){
    if(!pushingCards || pushedCards.empty())
    {
        pushingCards = false;
        return;
    }
    bool isMoveEnd = false;
    for(Card *card : pushedCards){
        sf::Vector2f directionVector = Card::getDirectionVector(card->moveDirection);
        card->move(2 * directionVector.x, 2 * directionVector.y);
        if(card->targetPosition == card->getPosition())isMoveEnd = true;
    }
    if(isMoveEnd)finishMove();
}

void setAnimation(Card& card,  Direction direction)
{
    animatedCards.push_back(&card);
    sf::Vector2f directionVector = Card::getDirectionVector(direction); 
    sf::Vector2f nextCardPosition = card.fieldPosition;
    nextCardPosition.x += Board::boardFieldSize * directionVector.x;
    nextCardPosition.y += Board::boardFieldSize * directionVector.y;
    sf::Vector2f nextCardFieldNumber = board->getCardFieldNumber(nextCardPosition);
    card.targetPosition = nextCardPosition;
    card.moveDirection = direction;
    if(board->isFieldEmpty(nextCardFieldNumber))
    {
        return;
    }
    else
    {
        setAnimation(*board->getFieldContent(nextCardFieldNumber), direction);
    }
}

void  playAnimation(){
    bool finishAnimation = false;
    if(animatedCards.empty())return;
    for(Card *card : animatedCards)
    {
        sf::Vector2f directionVector = Card::getDirectionVector(card->moveDirection);
        card->move(2 * directionVector.x, 2 * directionVector.y);
        if(card->targetPosition == card->getPosition())finishAnimation = true;
    }

    if(finishAnimation)
    {
        animationTimer.restart();
        animationState++;
    }
}



void restartAnimation()
{
    if(!playingAnimation)return;
    playingAnimation = false;
    if(animatedCards.empty())return;
    for(Card *card : animatedCards)
    {
        card->goToDeafultPosition();
        card->targetPosition = sf::Vector2f(0, 0);
        card->moveDirection = invalid;
    }
    animatedCards.clear();
    animationState = 0;
}

void restartGame()
{
    isEnd = false;
    for(int i = 0; i < sizeof(bluePlayerHandCards); i++)
    {
        bluePlayerHandCards[i] = NULL;
    }
    for(int i = 0; i < sizeof(redPlayerHandCards); i++)
    {
        redPlayerHandCards[i] = NULL;
    }
    selectedCard = NULL;
    focusCard = NULL;
    for(auto card : cards)
    {
        card.second->setPosition(sf::Vector2f(10000, 10000));
    }
    gems.clear();
    board->reset();
    
}


//close function

void close()
{
    onlineManager->broadcastExit();
    for(auto card : cards)
    {
       delete card.second;
    }
    cards.clear();
    pushedCards.clear();
    animatedCards.clear();
    delete blueCardsTilemap;
    delete redCardsTilemap;
    for(sf::Sprite *gem : gems)
    {
        delete gem;
    }
    gems.clear();
    delete gemTexture;
    delete board;
    delete onlineManager;

}

//main game loop functions

void render()
{
    window->clear();
    window->draw(*board);
    for(auto card : cards)
    {
        if(card.second == bluePlayerHandCards[activeCardIndex] && isBlue && isMyTurn) continue;
        else if(card.second == redPlayerHandCards[activeCardIndex] && !isBlue && isMyTurn)continue;
        window->draw(*card.second);
    }
    for(sf::Sprite* gem : gems)
    {
        window->draw(*gem);
    }
    if(bluePlayerHandCards[activeCardIndex] && isBlue && isMyTurn) window->draw(*bluePlayerHandCards[activeCardIndex]);
    else if(redPlayerHandCards[activeCardIndex] && !isBlue && isMyTurn) window->draw(*redPlayerHandCards[activeCardIndex]);
    if(isMyTurn)window->draw(focusBorder);
    if(isEnd)window->draw(gameEndText);
    window->display();
}

void input(sf::Event pending)
{
    /*if(isEnd)
    {
        if(pending.key.code == sf::Keyboard::Escape)
        {
            restartGame();
            onlineManager->broadcastGameRestart();
        }
    }*/
    if(pushingCards || !isMyTurn)return;
    if(pending.key.code == sf::Keyboard::Space)
    {
        if(focusCard != NULL)
        {
            selectedCard = focusCard;
            focusCard = NULL;
            selectedCard->setPosition(sf::Vector2f(Board::centerCardPositionX, Board::centerCardPositionY));
            selectedCard->fieldPosition.x = Board::centerCardPositionX;
            selectedCard->fieldPosition.y = Board::centerCardPositionY;

        }
        else if(selectedCard != NULL)
        {
            sf::Vector2f cardFieldPosition = board->getCardFieldNumber(selectedCard->fieldPosition);
            if(board->isFieldEmpty(cardFieldPosition))
            {
                for(sf::Vector2f position : board->gemsPositions)
                {
                    if(position == selectedCard->fieldPosition)
                    {
                        returnCardToHand();
                        return;
                    }
                }
                placeSelectedCardOnBoard(invalid);
            }
            else if(selectedCard->getPosition() != selectedCard->fieldPosition)
            {
                Direction direction = Card::getPushDirection(*selectedCard);
                Card& pushedCard = *board->getFieldContent(board->getCardFieldNumber(selectedCard->fieldPosition));
                if(canPush(direction, pushedCard , selectedCard->getArrow(direction)))
                {
                    //std::cout << pushedCard.fieldPosition.y << "\n" << pushedCard.deafultPosition.y << "\n";
                    restartAnimation();
                    setPushCards(pushedCard, direction);
                    pushingCards = true;
                }
                else
                {
                    restartAnimation();
                    returnCardToHand();
                }
            }
            else
            {
                restartAnimation();
                returnCardToHand();
            }
        }
    }

    if(pending.key.code == sf::Keyboard::W)
    {

        if(selectedCard != NULL)
        {
            if(board->isFieldEmpty(board->getCardFieldNumber(selectedCard->fieldPosition)) || selectedCard->getPosition().y < selectedCard->fieldPosition.y)
            {
                selectedCard->moveCard(up);
                restartAnimation(); 
            }
            else if(selectedCard->getPosition().y > selectedCard->fieldPosition.y)
            {
                selectedCard->setPosition(selectedCard->fieldPosition);
                restartAnimation();
            }
            else 
            {
               selectedCard->moveVisuallyCard(up);
               restartAnimation();
            }
        }
        else if(focusCard != NULL)
        {
            if(activeCardIndex == 0)
            {
                activeCardIndex = 2;
            }
            else
            {
                activeCardIndex--;
            }
            if(isBlue)focusCard = bluePlayerHandCards[activeCardIndex];
            else focusCard = redPlayerHandCards[activeCardIndex];
        }
    }

    if(pending.key.code == sf::Keyboard::S)
    {
        if(selectedCard != NULL)
        {
            if(board->isFieldEmpty(board->getCardFieldNumber(selectedCard->fieldPosition)) || selectedCard->getPosition().y > selectedCard->fieldPosition.y)
            {
                selectedCard->moveCard(down);
                restartAnimation();
            }
            else if(selectedCard->getPosition().y < selectedCard->fieldPosition.y)
            {
                selectedCard->setPosition(selectedCard->fieldPosition);
                restartAnimation();
            }
            else
            {
                selectedCard->moveVisuallyCard(down);
                restartAnimation();
            }
        }
        else if(focusCard != NULL)
        {
            if(activeCardIndex == 2)
            {
                activeCardIndex = 0;
            }
            else
            {
                activeCardIndex++;
            }
            if(isBlue)focusCard = bluePlayerHandCards[activeCardIndex];
            else focusCard = redPlayerHandCards[activeCardIndex];
        }
       
    }

    if(pending.key.code == sf::Keyboard::A && selectedCard != NULL)
    { 
        if(board->isFieldEmpty(board->getCardFieldNumber(selectedCard->fieldPosition)) || selectedCard->getPosition().x < selectedCard->fieldPosition.x)
        {
            selectedCard->moveCard(left);
            restartAnimation();
        }
        else if(selectedCard->getPosition().x > selectedCard->fieldPosition.x)
        {
            selectedCard->setPosition(selectedCard->fieldPosition);
            restartAnimation();
        }
        else
        {
            selectedCard->moveVisuallyCard(left);
            restartAnimation();
        }
    }

    if(pending.key.code == sf::Keyboard::D && selectedCard != NULL)
    {
         if(board->isFieldEmpty(board->getCardFieldNumber(selectedCard->fieldPosition)) || selectedCard->getPosition().x > selectedCard->fieldPosition.x)
        {
            selectedCard->moveCard(right);
            restartAnimation();
        }
        else if(selectedCard->getPosition().x < selectedCard->fieldPosition.x)
        {
            selectedCard->setPosition(selectedCard->fieldPosition);
            restartAnimation();
        }
        else
        {
            selectedCard->moveVisuallyCard(right);
            restartAnimation();
        }
    }

    if(pending.key.code == sf::Keyboard::Escape && selectedCard != NULL)
    {
        std::cout << board->isFieldEmpty(board->getCardFieldNumber(selectedCard->fieldPosition));
        returnCardToHand();
    }

}

void logic()
{

    if(pushingCards){
        pushCards();
        return;
    }

    if(focusCard != NULL)
    {
        focusBorder.setOutlineColor(sf::Color::Red);
        focusBorder.setPosition(focusCard->getPosition());
    }
    else
    {
        focusBorder.setOutlineColor(sf::Color::Transparent);
    }

    if(selectedCard != NULL)
    {
        sf::Vector2f selectedCardFieldNumber = board->getCardFieldNumber(selectedCard->fieldPosition);
        if(!board->isFieldEmpty(selectedCardFieldNumber) && selectedCard->getPosition() != selectedCard->fieldPosition)
        {
            Card& cardToAnimate = *board->getFieldContent(selectedCardFieldNumber);
            Direction animationDirection = Card::getPushDirection(*selectedCard);
            if(!playingAnimation)
            {
                if(canPush(animationDirection, cardToAnimate, selectedCard->getArrow(animationDirection))){
                    playingAnimation = true;
                    animationTimer.restart();
                    setAnimation(cardToAnimate ,animationDirection);
                }
            }
        }
        if(playingAnimation)
        {
            if(animationTimer.getElapsedTime().asSeconds() > 1 && (animationState == 0 || animationState == 2)){
                if(animationState == 0)animationState = 1;
                else animationState = 3;
            }

            switch (animationState)
            {
                case 1:
                    playAnimation();
                    break;
                case 3:
                    restartAnimation();
                    break;
                default:
                    break;
            }
        }
    }
}



// main function

int main(int argc, char** argv)
{
    --argc;
    ++argv;
    if(argc != 3){
        std::cout << "Usage: " << *(argv-1) << " <ip> <port>" << std::endl;
        return 0;
    }

    char* hostname = argv[0];
    uint16_t port = atoi(argv[1]);
    char* key = argv[2];
  

    srand(time(NULL));

    //Create game window
    window = new sf::RenderWindow(sf::VideoMode(640, 480), "Joustus");
    window->setKeyRepeatEnabled(false);

    //Load board
    sf::Texture boardTexture;
    boardTexture.loadFromFile("assets/Joustus-3X3 2.png");
    Board::centerCardPositionX = window->getSize().x/2 - (Card::cardSize/2);
    Board::centerCardPositionY = window->getSize().y/2 - (Card::cardSize/2);
    board = new Board(boardTexture);
    board->setPosition(sf::Vector2f(window->getSize().x/2 - (board->width/2), window->getSize().y/2 - (board->height/2)));

    // Create event
    sf::Event pending;
    // Event created
    sf::Font font;
    font.loadFromFile("fonts/Seagram.ttf");

    gameEndText.setFont(font);
    gameEndText.setFillColor(sf::Color::White);
    gameEndText.setOutlineColor(sf::Color::White);
    gameEndText.setString("Blue Win!");
    gameEndText.setCharacterSize(30);
    gameEndText.setPosition(sf::Vector2f(window->getSize().x/2 - ((gameEndText.getString().getSize()*15)/2), 0));

    //Set deafult positions for cards in player hand
    bluePlayerHandCardPositions[0] = sf::Vector2f(Board::boardFieldSize * 0.26f, window->getSize().y - Board::boardFieldSize*5.5);
    bluePlayerHandCardPositions[1] = sf::Vector2f(Board::boardFieldSize * 0.26f, window->getSize().y - Board::boardFieldSize*3.5);
    bluePlayerHandCardPositions[2] = sf::Vector2f(Board::boardFieldSize * 0.26f, window->getSize().y - Board::boardFieldSize*1.5);

    redPlayerHandCardPositions[0] = sf::Vector2f(window->getSize().x - Board::boardFieldSize * 0.26f - Card::cardSize, window->getSize().y - Board::boardFieldSize*5.5);
    redPlayerHandCardPositions[1] = sf::Vector2f(window->getSize().x - Board::boardFieldSize * 0.26f - Card::cardSize, window->getSize().y - Board::boardFieldSize*3.5);
    redPlayerHandCardPositions[2] = sf::Vector2f(window->getSize().x - Board::boardFieldSize * 0.26f - Card::cardSize, window->getSize().y - Board::boardFieldSize*1.5);
    
    setTextures();


    /*for(int i = 0; i < 3; i++)
    {
        playerOneHandCards[i] = drawCard();
        playerOneHandCards[i]->setPosition(playerOneHandCardPositions[i]);
        playerOneHandCards[i]->deafultPosition = playerOneHandCardPositions[i];
    }*/


    activeCardIndex = 0;

    focusBorder.setSize(sf::Vector2f(Card::cardSize, Card::cardSize));
    focusBorder.setOutlineThickness(Card::cardSize*0.05);
    focusBorder.setFillColor(sf::Color::Transparent);
    focusBorder.setOutlineColor(sf::Color::Transparent);


    focusCard = bluePlayerHandCards[0];

    float lastFrame = 0.0f;
    const float frameTime = 0.02f;

    sf::Clock timer;

    std::thread network(receiveFromServer, hostname, port, key);

    //main game loop

    while(window->isOpen()){
        while(window->pollEvent(pending)){
            if(pending.type == sf::Event::Closed){
                window->close();
            }
            if(pending.type == sf::Event::KeyPressed)
            {
                input(pending);
            }
        }
        if(!window->isOpen())break;

        float time = timer.getElapsedTime().asSeconds();
        while(lastFrame < time){
            logic();
            lastFrame += frameTime;
        }
        render();
    }
    close();
    network.join();
    return 0;
}