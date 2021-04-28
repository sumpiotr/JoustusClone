#include "onlineManager.h"


OnlineManager::OnlineManager(const char* address, uint16_t port, const char* key)
{
    socket.connect(sf::IpAddress(address), port);
    sf::Packet packet;
    packet << key;
    socket.send(packet);
}

sf::Packet OnlineManager::listen()
{
    sf::Packet packet;
    socket.receive(packet);
    return packet;
}

void OnlineManager::broadcastExit(){
    sf::Packet packet;
    packet << EVENT_EXIT;
    socket.send(packet);
}

void OnlineManager::broadcastTurnEnd(unsigned int id, sf::Vector2f fieldNumber, Direction direction){
    sf::Packet packet;
    packet << EVENT_MOVE_CARD;
    packet << id;
    packet << (int)fieldNumber.x;
    packet << (int)fieldNumber.y;
    packet << (int)direction;
    socket.send(packet);
}

void OnlineManager::broadcastGameEnd()
{
    sf::Packet packet;
    packet << EVENT_END_GAME;
    socket.send(packet);
}

void OnlineManager::broadcastGameRestart()
{
    sf::Packet packet;
    packet << EVENT_RESTART_GAME;
    socket.send(packet); 
}

void OnlineManager::broadcasNewGame()
{
    sf::Packet packet;
    packet << EVENT_NEW_GAME;
    socket.send(packet);
}