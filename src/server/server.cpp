#include <iostream>
#include <map>
#include <thread>
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/Packet.hpp>
#include "client.h"
#include "deck.h"
#include "room.h"

#include <algorithm>
#include <stdio.h>
#include <string.h>
#include <csignal>
#include <mutex>
#define c(x) std::cout << x << std::endl;

std::map <std::string, Client*> waiting;
std::vector<std::thread*> toClear;
std::vector<std::thread*> allThreads;
std::vector<Room*> rooms;
sf::TcpListener listener;

std::mutex gcm;

void handleConnection(sf::TcpSocket* socket, std::thread* self)
{
    c("Hello!!!");
    printf("%p\n", self);
    std::string key;
    sf::Packet packet;
    socket->receive(packet);
    packet >> key;
    c("Key is " << key);
    Client* client = new Client(socket, &waiting, key);
    client->startSelf();
    if(waiting.count(key))
    {
        Client* client1 = waiting[key];
        waiting.erase(key);
        Room* room = new Room(client, client1);
        rooms.push_back(room);
    }
    else
    {
        waiting[key] = client;
    }
    c("I am end");
    toClear.push_back(self);
}

bool running = true;

void end(int sig){
    std::cout << "Exiting with signal: " << sig << std::endl;
    running = false;
    gcm.unlock();
    listener.close();
}

void gc(){
    while(running){
        gcm.lock();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        for(auto i = toClear.begin(); i<toClear.end(); i++){
            delete *i;
        }

        for(auto a = rooms.begin(); a<rooms.end(); a++){
            if((*a)->isDeletable()) {
                delete *a;
                rooms.erase(a);
            }
        }

        toClear.clear();
    }
}

int main()
{
    srand(time(NULL));
    listener.setBlocking(true);
    listener.listen(1232);

    //std::thread garbageCollector(gc);

    //signal(SIGSEGV, end);
    signal(SIGINT, end);
    signal(SIGTERM, end);

    Room::blueDeck = new Deck();
    Room::blueDeck->addCard(0, 0, 0, 1, 0, 0, 0);
    Room::blueDeck->addCard(1, 0, 0, 1, 0, 0, 0);
    Room::blueDeck->addCard(2, 0, 0, 1, 0, 0, 0);
    Room::blueDeck->addCard(3, 510, 0, 0, 0, 0, 1);
    Room::blueDeck->addCard(4, 510, 0, 0, 0, 0, 1);
    Room::blueDeck->addCard(5, 510, 0, 0, 0, 0, 1);
    Room::blueDeck->addCard(6, 510, 0, 0, 0, 0, 1);
    Room::blueDeck->addCard(7, 510, 0, 0, 0, 0, 1);
    Room::blueDeck->addCard(8, 510, 0, 0, 0, 0, 1);

    Room::redDeck = new Deck();
    Room::redDeck->addCard(9, 2040, 0, 1, 0, 0, 0);
    Room::redDeck->addCard(10, 2040, 0, 1, 0, 0, 0);
    Room::redDeck->addCard(11, 2040, 0, 1, 0, 0, 0);
    Room::redDeck->addCard(12, 1020, 0, 0, 1, 0, 0);
    Room::redDeck->addCard(13, 1020, 0, 0, 1, 0, 0);
    Room::redDeck->addCard(14, 1020, 0, 0, 1, 0, 0);
    Room::redDeck->addCard(15, 1020, 0, 0, 1, 0, 0);
    Room::redDeck->addCard(16, 1020, 0, 0, 1, 0, 0);
    Room::redDeck->addCard(17, 1020, 0, 0, 1, 0, 0);

    sf::TcpSocket *sock;
    while(running)
    {
        sock = new sf::TcpSocket();
        if(listener.accept(*sock) == sf::Socket::Done){
            std::thread *newThread = new std::thread(&handleConnection, sock, newThread);
            allThreads.push_back(newThread);
        }
    }

    for(std::vector<std::thread*>::iterator i = allThreads.begin(); i<allThreads.end(); i++){
        (*i)->detach();
        std::cout << "Emergency-deleted: " << *i << std::endl;
        delete *i;
    }

    delete Room::blueDeck;
    delete Room::redDeck;
}
