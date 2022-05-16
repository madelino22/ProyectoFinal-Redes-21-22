CC=g++
CFLAGS=-g -std=c++11 -lSDL2 -lSDL2_image -I.
DEPS = Tablero.h Texture.h game.h
OBJ = Tablero.o Texture.o game.o
LIBS=-lpthread 

%.o: %.cc $(DEPS)
	$(CC) -g -c -o $@ $< $(CFLAGS)

all: cuatro 

cuatro: $(OBJ) game.o
	g++ -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f *.o cuatro

