#include "room.h"

Deck* Room::blueDeck;
Deck* Room::redDeck;

Room::Room(Client* client1, Client* client2)
{
    this->client1 = client1;
    this->client2 = client2;
    client1->assign(this);
    client2->assign(this);
    isCardDefinied = false;
    setNewGame();
    
}

void Room::markAsDeletable(){
    client1->disconnect();
    client2->disconnect();
    deletable = true;
}

Client* Room::getOppositeClient(Client* client)
{
    return client1 == client ? client2 : client1;
}

bool Room::isDeletable(){ return deletable; }

Room::~Room(){
    delete client1;
    delete client2;
    delete this;
}

void Room::defineCards()
{
    if(!isCardDefinied)
    {
         for(Card* card : client1->deck->cards)
        {
            client1->defineCard(*card, true);
            client2->defineCard(*card, false);
        }

        for(Card* card : client2->deck->cards)
        {
            client1->defineCard(*card, false);
            client2->defineCard(*card, true);
        }
        isCardDefinied = true;
    }
   


    for(int i = 0; i<3; i++){
        unsigned int id = client1->deck->getRandomCardId();
        client1->addCardToPlayers(id, true);
    }

    for(int i = 0; i<3; i++){
        unsigned int id = client2->deck->getRandomCardId();
        client2->addCardToPlayers(id, true);
    }
}

void Room::setGems()
{
    int randomPositionX;
    int randomPositionY;
    for(int i = 0; i < 3; i++)
    {
        randomPositionX = rand()%3 + 1;
        randomPositionY = rand()%3 + 1;       
        client1->setGem(randomPositionX, randomPositionY);
        client2->setGem(randomPositionX, randomPositionY);
    }
}



void Room::setNewGame()
{
    setGems();
    bool isClient1Blue = (rand()%2);
    client1->setColor(isClient1Blue);
    client2->setColor(!isClient1Blue);
    if(isClient1Blue)
    {
        client1->deck = new Deck(*blueDeck);
        client2->deck = new Deck(*redDeck);
    }
    else 
    {
        client2->deck = new Deck(*blueDeck);
        client1->deck = new Deck(*redDeck);
    }  
    defineCards();    
    (isClient1Blue ? client1 : client2) -> sendTurnStart();
}