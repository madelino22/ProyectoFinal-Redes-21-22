#include "Partida.h"
#include <string>
#include <Chat.h>


Partida::Partida(Socket* p1, Socket* p2, Socket* s, int _id): player1(p1), player2(p2), server(s), id(_id){
    //MENSAJES A LOS DOS SOCKETS PARA QUE INICIALICEN PARTIDA (Creen tablero, etc)
    std::string msg = " ";
    std::string nick = "";

    ChatMessage em(nick, msg);
    em.type = ChatMessage::STARTGAMEPLAYER1;

    server->send(em, *player1);

    ChatMessage em2(nick, msg);
    em2.type = ChatMessage::STARTGAMEPLAYER2;

    server->send(em2, *player2);
}

void Partida::addFicha(int jugador, int pos){
    //Mensaje el OTRO jugador para que sepa qué ficha ha añadido su oponente.
    if (jugador == 1){
        //mensaje a player2 sobre lo que ha movido el jugador 1
        
    }
    else{
        //mensaje a player1 sobre lo que ha movido el jugador 2
    }
}