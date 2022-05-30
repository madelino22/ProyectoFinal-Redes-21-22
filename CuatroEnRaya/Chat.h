#ifndef CHAT_H
#define CHAT_H

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
 *  | Tipo: uint8_t     | 0 (login), 1 (mensaje), 2 (logout)
 *  +-------------------+
 *  | Nick: char[8]     | Nick incluido el char terminación de cadena '\0'
 *  +-------------------+
 *  |                   |
 *  | Mensaje: char[80] | Mensaje incluido el char terminación de cadena '\0'
 *  |                   |
 *  +-------------------+
 *
 */
class Game;

class ChatMessage: public Serializable
{
public:
    static const size_t MESSAGE_SIZE = sizeof(char) * 88 + sizeof(uint8_t);

    enum MessageType
    {
        LOGIN   = 0,
        MESSAGE = 1,
        LOGOUT  = 2,
        STARTGAMEPLAYER1 = 3,
        STARTGAMEPLAYER2 = 4,
        ENDGAME = 5
    };

    ChatMessage(){};

    ChatMessage(const std::string& n, const int p):nick(n),pos(p){};

    void to_bin();

    int from_bin(char * bobj);

    uint8_t type;

    std::string nick;
    int pos;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 *  Clase para el servidor de chat
 */
static const int MAX_GAMES = 5;
class Partida;

class ChatServer
{
public:
    ChatServer(const char * s, const char * p): socket(s, p)
    {
        socket.bind();
        for (int i = 0; i < MAX_GAMES; ++i) {
            partidasEmpezadas.push_back(false);
            partidas.push_back(nullptr);
            partidasIDS.push_back(std::pair<int,int>(-1, -1));
        }
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
    std::vector<std::unique_ptr<Socket>> clients;
    std::queue<std::pair<Socket*, int>> matchmaking;


    /**
     * Socket del servidor
     */
    Socket socket;

    std::vector<bool> partidasEmpezadas;

    std::vector<Partida*> partidas;
    std::vector<std::pair<int, int>> partidasIDS;

    void comprobarMatchmaking();

    int NuevaPartida(Socket* p1, int p1ind, Socket* p2, int p2ind);
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 *  Clase para el cliente de chat
 */
class ChatClient
{
public:
    /**
     * @param s dirección del servidor
     * @param p puerto del servidor
     * @param n nick del usuario
     */
    ChatClient(const char * s, const char * p, const char * n):socket(s, p),
        nick(n){};

    /**
     *  Envía el mensaje de login al servidor
     */
    void login();

    /**
     *  Envía el mensaje de logout al servidor
     */
    void logout();

    /**
     *  Rutina principal para el Thread de E/S. Lee datos de STDIN (std::getline)
     *  y los envía por red vía el Socket.
     */
    void input_thread();

    /**
     *  Rutina del thread de Red. Recibe datos de la red y los "renderiza"
     *  en STDOUT
     */
    void net_thread();

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
};

#endif