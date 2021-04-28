#include "client.h"
#include <iostream>

Client::Client(sf::TcpSocket* socket, std::map<std::string, Client*> *waiting, std::string selfKey)
{
    this->socket = socket;
    this->waiting = waiting;
    this->waitingKey = selfKey;
}

void Client::assign(Room* room)
{
    this->room = room;
}

void Client::handleConnection(Client* ref)
{
    sf::Packet packet;
    while(true)
    {
        if(ref->socket->receive(packet) == sf::Socket::Disconnected){
            ref->closed = true;
            if(ref->room){
                ref->room->markAsDeletable();
            }else{
                ref->waiting->erase(ref->waitingKey);
                std::cout << "Erased self" << std::endl;
            }
            break;
        }
        int event;
        packet >> event;
        sf::Packet dolly(packet);
        switch(event)
        {
            case EVENT_MOVE_CARD:
            {
                ref->room->getOppositeClient(ref)->socket->send(dolly);
                unsigned int id = ref->deck->getRandomCardId();
                ref->addCardToPlayers(id, true);
                ref->room->getOppositeClient(ref)->sendTurnStart();
                break;
            }
            case EVENT_END_GAME:{
                ref->room->getOppositeClient(ref)->socket->send(dolly);
                break;
            }
            case EVENT_RESTART_GAME:
            {
                ref->room->getOppositeClient(ref)->socket->send(dolly);
                break;
            }
            case EVENT_NEW_GAME:
            {
                ref->room->setNewGame();
            }
            case EVENT_EXIT:
            {
                sf::Packet dolly2(dolly);
                ref->room->getOppositeClient(ref)->socket->send(dolly);
                ref->socket->send(dolly2);
                ref->socket->disconnect();
                goto exit;
            }
        }
    }
    exit:
    ;
}

void Client::startSelf(){
    selfThread = new std::thread(handleConnection, this);
}

void Client::setColor(bool isBlue)
{
    sf::Packet packet;
    packet << EVENT_SET_COLOR;
    packet << isBlue;
    socket->send(packet);
}

void Client::sendTurnStart()
{
    sf::Packet packet;
    packet << EVENT_TURN_START;
    socket->send(packet);
}

void Client::defineCard(Card& card, bool isMine)
{
    sf::Packet packet;
    packet << EVENT_DEFINE_CARD;
    packet << isMine;
    packet << card.id;
    packet << card.offsetX;
    packet << card.offsetY;
    packet << card.topArrow;
    packet << card.rightArrow;
    packet << card.bottomArrow;
    packet << card.leftArrow;
    socket->send(packet);
}

void Client::disconnect(){
    if(closed) return;
    closed = true;
    sf::Packet kill;
    kill << EVENT_EXIT;
    socket->send(kill);
    socket->disconnect();
}

Client::~Client(){
    disconnect();
    selfThread->join();
    delete selfThread;
}

void Client::addCardToPlayers(unsigned int id, bool isMine)
{
    addCardToPlayer(isMine, id);
    room->getOppositeClient(this)->addCardToPlayer(!isMine, id);
}

void Client::addCardToPlayer(bool isMine, unsigned int id)
{
    sf::Packet packet;
    packet << EVENT_ADD_PLAYER_CARD;
    packet << id;
    packet << isMine;
    socket->send(packet);
}

void Client::setGem(int positionX, int positionY)
{
    sf::Packet packet;
    packet << EVENT_SET_GEM;
    packet << positionX;
    packet << positionY;
    socket->send(packet);
}

void setDecks();
