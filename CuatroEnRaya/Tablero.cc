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
        encontrado = thisColum[i] != -1; 

        if (!encontrado) i++;
    }
    
    if (i == 0) return false;

    thisColum[i - 1] = jugador;

    final = checkGame(columna, i - 1);

    return true;
}

void Tablero::render(){
    cout << "Render de tablero" << std::endl;
    for (int i = 0; i < tablero.size(); ++i){
        for (int j = 0; j < tablero[i].size(); ++j){
            fichaRect.x = 85 + fichaRect.w * i;
            fichaRect.y = 30 + fichaRect.h * j;

            if (tablero[i][j] == 0)         fichaVaciaTex->render(fichaRect);
            else if (tablero[i][j] == 1)    fichaP1Tex->render(fichaRect);
            else /* == 2*/                  fichaP2Tex->render(fichaRect);

        }
    }
    cout << "Render de tablero terminada" << std::endl;
}

bool Tablero::checkGame(int columna, int fila){
    return false;
}