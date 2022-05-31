#include "game.h"
#include "Socket.h"
#include "Matchmaking.h"

Game::Game(int _jugador, Socket* p, std::string n){
    playerS = p;
    myNick = n;

    SDL_Init(SDL_INIT_EVERYTHING);
    std::string title = "CUATRO EN RAYA: " + myNick;
	window_ = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIN_WIDTH, WIN_HEIGHT, SDL_WINDOW_SHOWN);
	renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
	if (window_ == nullptr || renderer_ == nullptr) throw "Error loading the SDL window or renderer";

	cout << "Loading textures..." << std::endl;
	for (int i = 0; i < NUM_TEXTURES; i++) {
		texturas_[i] = new Texture(renderer_, ("Assets/" + atributes_[i].nombre), atributes_[i].row, atributes_[i].col);
	}

    cout << "Creando tablero" << std::endl;
    tablero = new Tablero(texturas_[fichaVacia], texturas_[fichaJugador1], texturas_[fichaJugador2]);

    jugador = _jugador;
    esMiTurno = jugador == 1;
    turnos = 0;

    seguirJugando = true;
}

Game::~Game(){
    delete tablero;

    for (int i = 0; i < NUM_TEXTURES; i++) delete texturas_[i];

	SDL_DestroyRenderer(renderer_);
	SDL_DestroyWindow(window_);
	SDL_Quit();
}

//Bucle principal del juego. Devuelve si quieres seguir jugando o no (dependiendo de si eres tú quien se ha salido del juego)
bool Game::run(){
    cout << "Abriendo Cuatro en Raya." << std::endl;
    //render();

    while(!exit){
        render();
        handleEvents();
        //cout << "playing" << std::endl;
    }

    cout << "Cerrando Cuatro en Raya" << std::endl;

    return seguirJugando;
}
	
//Renderización
void Game::render(){
    SDL_RenderClear(renderer_);
    tablero->render();
    SDL_RenderPresent(renderer_);
}

//Manejador de input
void Game::handleEvents(){
    SDL_Event evt;
	while (SDL_PollEvent(&evt) && !exit) {
		switch (evt.type)
        {
        case SDL_KEYDOWN:
            switch (evt.key.keysym.sym)
            {
                case SDLK_1:
                    if (esMiTurno) fichaPuesta = tablero->addFicha(0, jugador, final);
                    break;
                case SDLK_2:
                    if (esMiTurno) fichaPuesta = tablero->addFicha(1, jugador, final);
                    break;
                case SDLK_3:
                    if (esMiTurno) fichaPuesta = tablero->addFicha(2, jugador, final);
                    break;
                case SDLK_4:
                    if (esMiTurno) fichaPuesta = tablero->addFicha(3, jugador, final);
                    break;
                case SDLK_5:
                    if (esMiTurno) fichaPuesta = tablero->addFicha(4, jugador, final);
                    break;
                case SDLK_6:
                    if (esMiTurno) fichaPuesta = tablero->addFicha(5, jugador, final);
                    break;
                case SDLK_7:
                    if (esMiTurno) fichaPuesta = tablero->addFicha(6, jugador, final);
                    break;
                case SDLK_RETURN:
                    if (esMiTurno && fichaPuesta) pasaTurno();
                    break;
                case SDLK_ESCAPE:
                    cerrarJuego();
                    break;
                case SDL_WINDOWEVENT_CLOSE:
                    cerrarJuego();
                    break;
                default: 
                    break;
            }
            break;

        default:
            break;
        }
	}
}

//Devuelve el nº del jugador contrario (1 ó 2)
int Game::jugadorContrario(){
    if (jugador == 1) return jugador + 1;
    else return jugador - 1;
}

//Actualiza la variable de cantidad de turnos. Si se ha llegado a 42 reinicia el juego (se ha llenado el tablero)
void Game::cambiaTurnos(){
    if (!final) turnos++;
    
    if (turnos >= 42 || final){
        tablero->reset();
        turnos = 0;
    }
}

//Pasas el turno al jugador contrario enviando un mensaje con la posición de tu nueva ficha.
void Game::pasaTurno(){
    checkGame(jugador);

    int pos = tablero->pasaTurno(final);

    GameMessage em(myNick, pos);
    em.type = GameMessage::MESSAGE;

    playerS->send(em, *playerS);

    fichaPuesta = false;
    esMiTurno = false;

    

    cambiaTurnos();  
}

//Método que espera el mensaje del oponente, ya que es su turno
bool Game::recibirMensaje(){
    GameMessage em;
    playerS->recv(em);
    //std::cout << "Se ha recibido un mensaje" << std::endl;

    if (em.type != GameMessage::MESSAGE && em.type != GameMessage::ENDGAME && em.type != GameMessage::CLOSE) 
        std::cout << "FALLO EN LOS MENSAJES: no debería de haber llegado este tipo de mensaje aquí" << std::endl;

    else if (em.type == GameMessage::MESSAGE) //FIcha puesta por el adversario
    {
        if (!esMiTurno){
            if (otherNick == "") otherNick = em.nick;
            tablero->addFicha(em.pos, jugadorContrario(), final);
            checkGame(jugadorContrario());

            tablero->resetAnterior();
            cambiaTurnos();
            esMiTurno = true;
        }
    }
    else if (em.type == GameMessage::ENDGAME || em.type == GameMessage::CLOSE)
    //Recibes ENDGAME si el oponente ha cerrado el juego y CLOSE si eres tú quien ha decidido dejar de jugar.
    {     
        exit = true;
        return false;
    }
    return true;
        
}

//Comprueba si el jugador j ha ganado la partida al colocar su última ficha
void Game::checkGame(int j){
    final = tablero->checkGame(j);
    if (final) {
        if (j == jugador) std::cout << "¡Has ganado! ¡Enhorabuena " << myNick << "!" << std::endl;
        else std::cout << "Has perdido. Ha ganado " << otherNick << "." << std::endl;
    }
}

//Método llamado cuando tú quieres salir del juego, enviando un mensaje de ENDGAME al adversario
void Game::cerrarJuego(){
    seguirJugando = false;

    std::cout << "Te has salido del juego." << std::endl;

    int pos;

    GameMessage em(myNick, pos);
    em.type = GameMessage::ENDGAME;

    playerS->send(em, *playerS);
}