#include "game.h"
#include "Socket.h"
#include "Chat.h"

Game::Game(int _jugador, Socket* p){
    playerS = p;

    SDL_Init(SDL_INIT_EVERYTHING);
	window_ = SDL_CreateWindow("CUATRO EN RAYA", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIN_WIDTH, WIN_HEIGHT, SDL_WINDOW_SHOWN);
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
}

Game::~Game(){
    delete tablero;

    for (int i = 0; i < NUM_TEXTURES; i++) delete texturas_[i];

	SDL_DestroyRenderer(renderer_);
	SDL_DestroyWindow(window_);
	SDL_Quit();
}

void Game::run(){
    cout << "running game" << std::endl;
    //render();

    while(!exit){
        render();
        handleEvents();
    }

    cout << "exit game" << std::endl;
}
	

void Game::render(){
    SDL_RenderClear(renderer_);
    tablero->render();
    SDL_RenderPresent(renderer_);
}


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

int Game::jugadorContrario(){
    if (jugador == 1) return jugador + 1;
    else return jugador - 1;
}

void Game::cambiaTurnos(){
    if (!final) turnos++;
    
    if (turnos >= 42 || final){
        tablero->reset();
        turnos = 0;
    }
}

void Game::pasaTurno(){
    checkGame(jugador);

    int pos = tablero->pasaTurno(final);
    std::string nick;

    ChatMessage em(nick, pos);
    em.type = ChatMessage::MESSAGE;

    playerS->send(em, *playerS);

    fichaPuesta = false;
    esMiTurno = false;

    

    cambiaTurnos();  
}

void Game::recibirMensaje(){
    ChatMessage em;
    playerS->recv(em);
    std::cout << "Se ha recibido un mensaje" << std::endl;

    if (em.type != ChatMessage::MESSAGE && em.type != ChatMessage::ENDGAME) std::cout << "MAAAAAAAAAAAAAAAAAAAAAAL" << std::endl;
    else if (em.type == ChatMessage::MESSAGE){
        if (!esMiTurno){
            tablero->addFicha(em.pos, jugadorContrario(), final);
            checkGame(jugadorContrario());

            tablero->resetAnterior();
            cambiaTurnos();
            esMiTurno = true;
        }
    }
    else //ENDGAME
    {
        exit = true;
        SDL_Quit();
    }
        
}

void Game::checkGame(int j){
    final = tablero->checkGame(j);
    if (final) std::cout << "Ha ganado el jugador " << j << "." << std::endl;
}

void Game::cerrarJuego(){
    std::string nick;
    int pos;

    ChatMessage em(nick, pos);
    em.type = ChatMessage::ENDGAME;

    playerS->send(em, *playerS);

    exit = true;
    SDL_Quit();
    delete this;
}