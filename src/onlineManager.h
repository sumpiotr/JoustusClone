#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/Packet.hpp>
#include "directionEnum.h"
#include "onlineEvents.h"
#include <iostream>

class OnlineManager
{
    public:
        OnlineManager(const char* address, uint16_t port, const char* key);
        void broadcastTurnEnd(unsigned int id, sf::Vector2f fieldNumber, Direction direction);
        void broadcastExit();
        void broadcastGameEnd();
        void broadcastGameRestart();
        void broadcasNewGame();
        sf::Packet listen();
    protected:
        sf::TcpSocket socket;
};


