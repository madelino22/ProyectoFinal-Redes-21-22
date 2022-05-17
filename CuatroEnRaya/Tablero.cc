#include "Tablero.h"

Tablero::Tablero(int ncolumnas, int nfilas, int num, Texture* f, Texture* p1t, Texture* p2t): 
    fichaVaciaTex(f), fichaP1Tex(p1t), fichaP2Tex(p2t)
{
    vector<int> fila(nfilas, num);

    for (int i = 0; i < ncolumnas; ++i){
        tablero.push_back(fila);
    }

    fichaRect.h = 90;
    fichaRect.w = 90;
}

bool Tablero::addFicha(int columna, int jugador, bool& final){
    bool encontrado = false;
    vector<int> thisColum = tablero[columna];
    int i = 0;

    while (!encontrado && i < thisColum.size()){
        encontrado = thisColum[i] != 0; 

        if (!encontrado) i++;
    }
    
    if (i == 0) return false;

    tablero[columna][i - 1] = jugador;

    final = checkGame(columna, i - 1, jugador);
    if (final) cout << "Ha ganado el jugador " << jugador << std::endl;
    return true;
}

void Tablero::render(){
    for (int i = 0; i < tablero.size(); ++i){
        for (int j = 0; j < tablero[i].size(); ++j){
            fichaRect.x = 85 + fichaRect.w * i;
            fichaRect.y = 30 + fichaRect.h * j;

            if (tablero[i][j] == 0)         fichaVaciaTex->render(fichaRect);
            else if (tablero[i][j] == 1)    fichaP1Tex->render(fichaRect);
            else /* == 2*/                  fichaP2Tex->render(fichaRect);

        }
    }
}

bool Tablero::checkGame(int columna, int fila, int jugador){
    bool horizontal = recursion(columna, fila, -1, 0, jugador) + recursion(columna, fila, 1, 0, jugador) >= 3;
    bool vertical = recursion(columna, fila, 0, 1, jugador) + recursion(columna, fila, 0, -1, jugador) >= 3;
    bool diagonalPpal = recursion(columna, fila, 1, -1, jugador) + recursion(columna, fila, -1, 1, jugador) >= 3;
    bool diagonalSec = recursion(columna, fila, 1, 1, jugador) + recursion(columna, fila, -1, -1, jugador) >= 3;

    return horizontal || vertical || diagonalPpal || diagonalSec;
}

int Tablero::recursion(int columna, int fila, int movX, int movY, int jugador){
    columna += movX; fila += movY;
    if (columna >= tablero.size() || columna < 0 || fila >= tablero[columna].size() || fila < 0) return 0;
    if (tablero[columna][fila] != jugador) return 0;
    return recursion(columna, fila, movX, movY, jugador) + 1;
}

void Tablero::reset(){
    for (int i = 0; i < tablero.size(); ++i){
        for (int j = 0; j < tablero[i].size(); ++j){
            tablero[i][j] = 0;
        }
    }
}