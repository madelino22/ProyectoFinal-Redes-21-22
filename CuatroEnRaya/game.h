#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include "Tablero.h"
#include "Texture.h"

using namespace std;

enum TexturesName{fichaVacia, fichaJugador1, fichaJugador2};
const int WIN_WIDTH = 800;
const int WIN_HEIGHT = 600;
const int NUM_TEXTURES = 3;

class Game{
    
private:
    SDL_Window* window_ = nullptr;
	SDL_Renderer* renderer_ = nullptr;
	bool exit = false;
	Texture* texturas_[NUM_TEXTURES];

    bool final = false;

	struct TexturesAtributes {
		string nombre;
		int row;
		int col;
	};

	TexturesAtributes atributes_[NUM_TEXTURES] = { "fichaVacia.png", 1, 1, "fichaJugador1.png", 1, 1, "fichaJugador2.png", 1, 1};

    Tablero* tablero;

	bool esMiTurno;
	void cambiaJugador();
	int jugador;
	void cambiaTurnos();
	int turnos;

public:
	Game(int _jugador);
	~Game();
	void run();
	void render();
	bool handleEvents();
};


int main(int argc, char **argv){
    Game game = Game(1);

    game.run();

	return 0;
}

