#ifndef PARTIDA_H
#define PARTIDA_H
//#include "Socket.h"
#include <string>

class Socket;

class Partida {
private:
    Socket* player1;
    Socket* player2;
    Socket* server;

    int indPl1;
    int indPl2;

    int id;

public:
    Partida(Socket* p1, int iP1, Socket* p2, int iP2, Socket* s, int _id);

    void addFicha(int jugador, int pos, std::string nick);
    int deleteGame(int jugador);

    Socket* getSocketPlayer1(){ return player1;};
    Socket* getSocketPlayer2(){ return player2;};
    int getIndicePlayer1(){ return indPl1;};
    int getIndicePlayer2(){ return indPl2;};
    int getIDPartida(){return id;}

    
};
#endif