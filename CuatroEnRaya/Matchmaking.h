#ifndef MATCHMAKING_H
#define MATCHMAKING_H

#include <string>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <memory>
#include <queue>

#include "Serializable.h"
#include "Socket.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

/**
 *  Mensaje del protocolo de la aplicación de Chat
 *
 *  +-------------------+
 *  | Tipo: uint8_t     | MessageType
 *  +-------------------+
 *  | Nick: char[8]     | Nick incluido el char terminación de cadena '\0'
 *  +-------------------+
 *  |                   |
 *  | pos: int          | Entero que indica posición de la nueva ficha o nº de jugaodor
 *  |                   |
 *  +-------------------+
 *
 */
class Game;

class GameMessage: public Serializable
{
public:
    static const size_t MESSAGE_SIZE = sizeof(char) * 88 + sizeof(uint8_t);

    enum MessageType
    {
        LOGIN   = 0,            //Nuevo cliente accediendo al servidor
        MESSAGE = 1,            //Mensaje principal del juego: se ha puesto una ficha
        LOGOUT  = 2,            //Cliente que se sale de la cola de espera de matchmaking
        STARTGAMEPLAYER = 3,    //Comienzo de nueva partida
        ENDGAME = 4,            //Enviado por cliente que se quiere salir de la partida (lo recibe el que no se quiere salir)
        CLOSE = 5,              //Mensaje enviado a quien se quiere salir de la partida para que se cierre su juego
        SERVERFULL = 6          //Cliente intentando acceder a servidor ya lleno de clientes
    };

    GameMessage(){};

    GameMessage(const std::string& n, const int p):nick(n),pos(p){};

    void to_bin();

    int from_bin(char * bobj);

    uint8_t type;

    std::string nick;           //Nombre del cliente
    int pos;                    //Puede actuar como posición de nueva ficha y como numero de jugador a la hora de crear partida
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 *  Clase para el servidor de chat
 */
static const int MAX_GAMES = 5;
static const int MAX_CLIENTS = 11; //Max games * 2 + 1 cliente en espera
class Partida;

class GameServer
{
public:
    GameServer(const char * s, const char * p): socket(s, p)
    {
        socket.bind();
        for (int i = 0; i < MAX_GAMES; ++i) {
            partidasEmpezadas.push_back(false);
            partidas.push_back(nullptr);
            
        }

        nClientes = 0;
    };

    /**
     *  Thread principal del servidor recive mensajes en el socket y
     *  lo distribuye a los clientes. Mantiene actualizada la lista de clientes
     */
    void do_messages();

private:
    int emparejar(Socket* s1, int p1ind, Socket* s2, int p2ind);

    /**
     *  Lista de clientes conectados al servidor de Chat, representados por
     *  su socket
     */
    std::vector<std::unique_ptr<Socket>> clients;       //Lista de todos los clientes que se han conectado al servidor
    int nClientes;                                      //Número de clientes actualmente conectados
    std::queue<std::pair<Socket*, int>> matchmaking;    //Cola de espera de matchmaking


    /**
     * Socket del servidor
     */
    Socket socket;

    std::vector<Partida*> partidas;                     //Vector de partidas
    std::vector<bool> partidasEmpezadas;                //Booleanos que indican qué partidas están empezadas o no en el vector de Partidas anterior

    /**
     * Vector igual de grande que el vector de clients
     * Indica en qué partida y qué jugador es (1 ó 2) cada cliente
     */
    std::vector<std::pair<int, int>> partidasIDS;       

    void comprobarMatchmaking();

    int NuevaPartida(Socket* p1, int p1ind, Socket* p2, int p2ind);
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 *  Clase para el cliente de chat
 */
class GameClient
{
public:
    /**
     * @param s dirección del servidor
     * @param p puerto del servidor
     * @param n nick del usuario
     */
    GameClient(const char * s, const char * p, const char * n):socket(s, p),
        nick(n), quieroSeguirJugando(true), jugando(false), game(nullptr){};

    /**
     *  Envía el mensaje de login al servidor
     */
    void login();

    /**
     *  Envía el mensaje de logout al servidor
     */
    void logout();

    /**
     *  Espera a mensaje de empezar partida o de que el servidor esté lleno.
     *  Si empieza la partida, es el thread que llevará el bucle principal de juego.
     */
    void net_thread();

    /**
     *  Si el juego ha empezado, se encarga de recibir los mensajes del oponente
     */
    void gameRecieve();

private:

    /**
     * Socket para comunicar con el servidor
     */
    Socket socket;

    /**
     * Nick del usuario
     */
    std::string nick;

    Game* game;

    bool jugando;               //Booleano que indica si estás jugando o no.
    bool quieroSeguirJugando;   //Booleano que indica si quieres seguir jugando o no. Se vuelve false si eres tú quien sale de la partida.
};

#endif