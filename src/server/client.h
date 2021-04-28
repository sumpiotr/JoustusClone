#pragma once
#include <thread>
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/Packet.hpp>
#include "../onlineEvents.h"
#include "deck.h"
#include "room.h"
#include <vector>
#include <map>

class Room;

class Client
{
    public:
        Client(sf::TcpSocket* socket, std::map<std::string, Client*> *waiting, std::string selfKey);
        void assign(Room* room);
        void startSelf();
        void setColor(bool isBlue);
        void sendTurnStart();
        void defineCard(Card& card, bool isMine);
        void addCardToPlayers(unsigned int id, bool isMine = true);
        void setGem( int positionX, int positionY);
        ~Client();
        void disconnect();
        Deck* deck;
    private:
        sf::TcpSocket* socket;
        Room* room;
        static void handleConnection(Client* ref);
        void addCardToPlayer(bool isMine, unsigned int id);
        std::thread* selfThread;
        std::map<std::string, Client*> *waiting;
        std::string waitingKey;
        bool closed = false;

};

