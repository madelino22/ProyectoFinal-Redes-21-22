#include "Partida.h"
#include <string>
#include <Chat.h>


Partida::Partida(Socket* p1, int iP1, Socket* p2, int iP2, Socket* s, int _id): player1(p1), indPl1(iP1), player2(p2), indPl2(iP2), server(s), id(_id){
    //MENSAJES A LOS DOS SOCKETS PARA QUE INICIALICEN PARTIDA (Creen tablero, etc)
    std::string nick = "";
    int pos = 0;

    ChatMessage em(nick, 0);
    em.type = ChatMessage::STARTGAMEPLAYER1;

    server->send(em, *player1);

    ChatMessage em2(nick, 0);
    em2.type = ChatMessage::STARTGAMEPLAYER2;

    server->send(em2, *player2);
}

void Partida::addFicha(int jugador, int pos){
    std::string nick = "";
    //Mensaje el OTRO jugador para que sepa qué ficha ha añadido su oponente.
    if (jugador == 1){
        //mensaje a player2 sobre lo que ha movido el jugador 1
        ChatMessage em(nick, pos);
        em.type = ChatMessage::MESSAGE;

        server->send(em, *player2);
        
    }
    else{
        //mensaje a player1 sobre lo que ha movido el jugador 2
        ChatMessage em(nick, pos);
        em.type = ChatMessage::MESSAGE;

        server->send(em, *player1);
    }
}

int Partida::deleteGame(int jugador){
    std::string nick = "";
    int pos;
    //Mensaje el OTRO jugador para que sepa qué ficha ha añadido su oponente.
    if (jugador == 1){
        //mensaje a player2 sobre lo que ha movido el jugador 1
        ChatMessage em(nick, pos);
        em.type = ChatMessage::ENDGAME;

        server->send(em, *player2);


        em.type = ChatMessage::CLOSE;


        server->send(em, *player1);

        return 2;
    }
    else{
        //mensaje a player1 sobre lo que ha movido el jugador 2
        ChatMessage em(nick, pos);
        em.type = ChatMessage::ENDGAME;

        server->send(em, *player1);
        //server->send(em, *player2);

        em.type = ChatMessage::CLOSE;


        server->send(em, *player2);

        return 1;
    }
}

