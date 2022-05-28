C=g++
CFLAGS=-g -std=c++11 -lSDL2 -lSDL2_image -I.
DEPS = Socket.h Chat.h Tablero.h Texture.h game.h Partida.h
OBJ = Socket.o Chat.o Tablero.o Texture.o game.o Partida.o
LIBS=-lpthread

%.o: %.cc $(DEPS)
	$(CC) -g -c -o $@ $< $(CFLAGS)

all: cs cc

cs: $(OBJ) ChatServer.o
	g++ -o $@ $^ $(CFLAGS) $(LIBS)

cc: $(OBJ) ChatClient.o
	g++ -o $@ $^ $(CFLAGS) $(LIBS)


.PHONY: clean

clean:
	rm -f *.o cs cc

