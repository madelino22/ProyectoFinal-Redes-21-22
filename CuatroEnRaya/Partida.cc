#include "Partida.h"
#include <string>
#include "Matchmaking.h"


Partida::Partida(Socket* p1, int iP1, Socket* p2, int iP2, Socket* s, int _id): player1(p1), indPl1(iP1), player2(p2), indPl2(iP2), server(s), id(_id){
    //MENSAJES A LOS DOS SOCKETS PARA QUE INICIALICEN PARTIDA (Creen tablero, etc)
    std::string nick = "";

    GameMessage em(nick, 1);
    em.type = GameMessage::STARTGAMEPLAYER;

    server->send(em, *player1);

    GameMessage em2(nick, 2);
    em2.type = GameMessage::STARTGAMEPLAYER;

    server->send(em2, *player2);
}

//Método que envía un mensaje a un jugador indicando qué ficha ha puesto su adversario.
void Partida::addFicha(int jugador, int pos, std::string nick){
    //Mensaje el OTRO jugador para que sepa qué ficha ha añadido su oponente.
    if (jugador == 1){
        //mensaje a player2 sobre lo que ha movido el jugador 1
        GameMessage em(nick, pos);
        em.type = GameMessage::MESSAGE;

        server->send(em, *player2);
        
    }
    else{
        //mensaje a player1 sobre lo que ha movido el jugador 2
        GameMessage em(nick, pos);
        em.type = GameMessage::MESSAGE;

        server->send(em, *player1);
    }
}

//Método que envía mensaje al jugador contrario cuando uno se ha salido.
//Devuelve el nº del jugador que quiere seguir jugando para devolverlo al matchmaking
int Partida::deleteGame(int jugador){
    std::string nick = "";
    int pos;
    //Mensaje el OTRO jugador para que sepa qué ficha ha añadido su oponente.
    if (jugador == 1){
        //mensaje a player2 sobre lo que ha movido el jugador 1
        GameMessage em(nick, pos);
        em.type = GameMessage::ENDGAME;

        server->send(em, *player2);


        em.type = GameMessage::CLOSE;


        server->send(em, *player1);

        return 2;
    }
    else{
        //mensaje a player1 sobre lo que ha movido el jugador 2
        GameMessage em(nick, pos);
        em.type = GameMessage::ENDGAME;

        server->send(em, *player1);
        //server->send(em, *player2);

        em.type = GameMessage::CLOSE;


        server->send(em, *player2);

        return 1;
    }
}

