#ifndef PARTIDA_H
#define PARTIDA_H
//#include "Socket.h"

class Socket;

class Partida {
private:
    Socket* player1;
    Socket* player2;
    Socket* server;

public:
    Partida(Socket* p1, Socket* p2, Socket* s, int _id);

    void addFicha(int jugador, int pos);

    int id;
};
#endif