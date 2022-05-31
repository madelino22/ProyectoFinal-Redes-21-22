#include "Chat.h"
#include <iostream>
#include "Partida.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

    //Serializar los campos type, nick y message en el buffer _data
    char* c = _data;
    memcpy(c, &type, sizeof(uint8_t));
    c += sizeof(uint8_t);
    memcpy(c, nick.c_str(),sizeof(char) * 8);
    c += 8 * sizeof(char);
    memcpy(c, &pos, sizeof(int));
}

int ChatMessage::from_bin(char * bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    char nameAux [8];
    int posAux;

    //Reconstruir la clase usando el buffer _data
    char* aux = _data;
    memcpy(&type, aux, sizeof(uint8_t));
    aux += sizeof(uint8_t);
    memcpy(&nameAux, aux, 8 * sizeof(char));
    aux += 8 * sizeof(char);
    memcpy(&posAux, aux, sizeof(int));

    nick = nameAux;
    pos = posAux;

    return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::do_messages()
{
    while (true)
    {
        /*
         * NOTA: los clientes están definidos con "smart pointers", es necesario
         * crear un unique_ptr con el objeto socket recibido y usar std::move
         * para añadirlo al vector
         */

        //Recibir Mensajes en y en función del tipo de mensaje
        // - LOGIN: Añadir al vector clients
        // - LOGOUT: Eliminar del vector clients
        // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)

        Socket* udpSocket;
        ChatMessage msg;
        //Recibir mensajes según tipo de mensaje
        socket.recv(msg, udpSocket);
        // LOGIN: Añadir al vector clients
        if(msg.type == ChatMessage::MessageType::LOGIN){
            if (nClientes < 11){
                std::unique_ptr<Socket> u_ptr(udpSocket);
                clients.push_back(std::move(u_ptr)); 
                matchmaking.push(std::pair<Socket*,int>(udpSocket, clients.size()-1));
                printf("%s login.\n", msg.nick.c_str());

                partidasIDS.push_back(std::pair<int,int>(-1, -1));

                nClientes++;

                comprobarMatchmaking();
            }
            else {
                std::string nick;
                int p;

                ChatMessage em(nick, p);
                em.type = ChatMessage::MessageType::SERVERFULL;

                socket.send(em, *udpSocket);
            }

            
        }
        // LOGOUT: Eliminar del vector clients
        else if(msg.type == ChatMessage::MessageType::LOGOUT){
            auto it = clients.begin();
            while(it != clients.end() && !(*(*it).get() == *udpSocket)) it++;
            clients.erase(it);
            printf("%s logout.\n", msg.nick.c_str());

            nClientes--;
        }
        // MESSAGE: Ficha del cuatro en raya puesta.
        else if(msg.type == ChatMessage::MessageType::MESSAGE){

            int i = 0; bool encontrado = false;
            while (i < clients.size() && !encontrado){
                encontrado = *udpSocket == *(clients[i].get());
                if (!encontrado) i++;
            }  

            partidas[partidasIDS[i].first]->addFicha(partidasIDS[i].second, msg.pos, msg.nick);
            std::cout << "En la partida " << partidasIDS[i].first << ", " << msg.nick <<" ha puesto una ficha en la pos " << msg.pos << std::endl;
        }
        else if (msg.type == ChatMessage::MessageType::ENDGAME){
            //CERRAR EL JUEGO
            int i = 0; bool encontrado = false;
            while (i < clients.size() && !encontrado){
                encontrado = *udpSocket == *(clients[i].get());
                if (!encontrado) i++;
            }  

            nClientes--;

            int numJugador = partidas[partidasIDS[i].first]->deleteGame(partidasIDS[i].second);
            partidasEmpezadas[partidasIDS[i].first] = false;


            //AÑADIR DE NUEVO AL MATCHMAKING
            Socket* sock;
            int indiceSock;

            if (numJugador == 1){
                sock = partidas[partidasIDS[i].first]->getSocketPlayer1();
                indiceSock = partidas[partidasIDS[i].first]->getIndicePlayer1();
            }
            else{
                sock = partidas[partidasIDS[i].first]->getSocketPlayer2();
                indiceSock = partidas[partidasIDS[i].first]->getIndicePlayer2();
            }

            matchmaking.push(std::pair<Socket*,int>(sock, indiceSock));
            comprobarMatchmaking();
        }
    }
}

void ChatServer::comprobarMatchmaking(){
    if (matchmaking.size() == 2) {
        Socket* player1 = matchmaking.front().first;
        int p1ind = matchmaking.front().second; 
        matchmaking.pop();
        Socket* player2 = matchmaking.front().first; 
        int p2ind = matchmaking.front().second; 
        matchmaking.pop();

        int partidaID = emparejar(player1, p1ind, player2, p2ind);

        partidasIDS[p1ind].first = partidaID;
        partidasIDS[p1ind].second = 1;

        partidasIDS[p2ind].first = partidaID;
        partidasIDS[p2ind].second = 2;
    }
}

int ChatServer::emparejar(Socket* s1, int p1ind, Socket* s2, int p2ind){
    int partidaElegida = NuevaPartida(s1, p1ind, s2, p2ind);
    if (partidaElegida != -1) std::cout << "Te han emparejado para una nueva partida" << std::endl; 
    else std::cout << "El servidor no puede empezar más partidas." << std::endl; 

    return partidaElegida;
}

int ChatServer::NuevaPartida(Socket* p1, int p1ind, Socket* p2, int p2ind){
        //Asignación al array
    int i = 0;
    while (i < MAX_GAMES && partidasEmpezadas[i]) i++;

    if (i >= MAX_GAMES) return -1; //TODAS LAS PARTIDAS ESTÁN LLENAS

    partidas[i] = new Partida(p1, p1ind, p2, p2ind, &socket, i);
    partidasEmpezadas[i] = true;
    return i;
}
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatClient::login()
{
    int p;

    ChatMessage em(nick, p);
    em.type = ChatMessage::LOGIN;

    socket.send(em, socket);
}

void ChatClient::logout()
{
    int p;

    ChatMessage em(nick, p);
    em.type = ChatMessage::LOGOUT;

    socket.send(em, socket);
}

void ChatClient::input_thread()
{
    while (true)
    {
    //     // Leer stdin con std::getline
    //     std::string inString;
    //     std::getline(std::cin, inString);
    //     if(inString == "q" || inString == "Q"){
    //         logout();
    //         break;
    //     }
    //     ChatMessage em(nick, inString);
    //     em.type = ChatMessage::MessageType::MESSAGE;
    //     // Enviar el mensaje al server usando el socket
    //     socket.send(em, socket);
    }
}
#include "game.h"

void ChatClient::gameRecieve(){
    while (quieroSeguirJugando){
        //std::cout << "entra while gameRecieve" << std::endl;
        if (jugando && game != nullptr){
            jugando = game->recibirMensaje();
        }

        //std::cout << "GR Quiero seguir jugando: " << quieroSeguirJugando << std::endl;
    }
}



void ChatClient::net_thread()
{
    while(quieroSeguirJugando)
    {
        //std::cout << "entra while net_thread" << std::endl;
        //Recibir mensaje
        ChatMessage em;
        socket.recv(em);

        if (em.type == ChatMessage::LOGOUT)
            break;

        else if (em.type == ChatMessage::STARTGAMEPLAYER1){
            jugando = true;
            game = new Game(1, &socket, nick);
            quieroSeguirJugando = game->run();
            delete game;
            game = nullptr;

            std::cout << "NT Quiero seguir jugando: " << quieroSeguirJugando << std::endl;
        }
        else if (em.type == ChatMessage::STARTGAMEPLAYER2){
            jugando = true;
            game = new Game(2, &socket, nick);
            quieroSeguirJugando = game->run();
            delete game;
            game = nullptr;

            std::cout << "NT Quiero seguir jugando: " << quieroSeguirJugando << std::endl;
        }
        else if (em.type == ChatMessage::SERVERFULL){
            std::cout << "El servidor está lleno. Vuelve a intentarlo más tarde." << std::endl;
            quieroSeguirJugando = false;
        }
    }
}
