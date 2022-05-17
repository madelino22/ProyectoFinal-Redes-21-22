#include "game.h"

Game::Game(int _jugador){
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
    render();

    while(!final){
        if (esMiTurno){
            if (handleEvents()) {
                cambiaTurnos();
                //cambiaJugador();
                render();
            }
        }    
    }
}
	

void Game::render(){
    SDL_RenderClear(renderer_);
    tablero->render();
    SDL_RenderPresent(renderer_);
}


bool Game::handleEvents(){
    SDL_Event evt;
	bool handled = false;
	while (SDL_PollEvent(&evt) && !exit) {
		switch (evt.type)
        {
        case SDL_QUIT:
            exit = true;
            break;
        case SDL_KEYDOWN:
            switch (evt.key.keysym.sym)
            {
                case SDLK_1:
                    handled = tablero->addFicha(0, jugador, final);
                    break;
                case SDLK_2:
                    handled = tablero->addFicha(1, jugador, final);
                    break;
                case SDLK_3:
                    handled = tablero->addFicha(2, jugador, final);
                    break;
                case SDLK_4:
                    handled = tablero->addFicha(3, jugador, final);
                    break;
                case SDLK_5:
                    handled = tablero->addFicha(4, jugador, final);
                    break;
                case SDLK_6:
                    handled = tablero->addFicha(5, jugador, final);
                    break;
                case SDLK_7:
                    handled = tablero->addFicha(6, jugador, final);
                    break;

                default: 
                    break;
            }
            break;

        default:
            break;
        }
	}

    return handled;
}

void Game::cambiaJugador(){
    if (jugador == 1) jugador++;
    else jugador--;
}

void Game::cambiaTurnos(){
    if (!final) turnos++;
    
    if (turnos >= 21){
        tablero->reset();
        turnos = 0;
    }
}