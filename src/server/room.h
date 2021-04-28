#pragma once
#include "deck.h"
#include "client.h"
#include "iostream"

class Client;

class Room
{
    public:
        Room(Client* client1, Client* client2);
        Client* getOppositeClient(Client* client);
        ~Room();
        void markAsDeletable();
        static Deck* blueDeck;
        static Deck* redDeck;
        bool isDeletable();
        void setNewGame();
    private: 
        Client* client1;
        Client* client2;
        void defineCards();
        void setGems();
        bool deletable;
        bool isCardDefinied;
};
