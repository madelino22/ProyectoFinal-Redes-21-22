#ifndef TABLERO_H
#define TABLERO_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include <iostream>
#include "Texture.h"

using namespace std;

class Tablero{

public:
    

    Tablero(Texture* f, Texture* p1t, Texture* p2t): Tablero(7, 6, 0, f, p1t, p2t){};
        
    bool addFicha(int columna, int jugador, bool& final);

    void render();

    void reset();

    bool checkGame(int jugador);

    int pasaTurno(bool& final);
    void resetAnterior();
private:
    Tablero(int ncolumnas, int nfilas, int num, Texture* f, Texture* p1t, Texture* p2t);

    

    int recursion(int columna, int fila, int movX, int movY, int jugador);

    vector<vector<int>> tablero;

    std::pair<int, int> fichaAnterior;

    SDL_Rect fichaRect;
    Texture* fichaVaciaTex;
    Texture* fichaP1Tex;
    Texture* fichaP2Tex;
};

#endif