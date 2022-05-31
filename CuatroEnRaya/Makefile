C=g++
CFLAGS=-g -std=c++11 -lSDL2 -lSDL2_image -I.
DEPS = Socket.h Matchmaking.h Tablero.h Texture.h game.h Partida.h
OBJ = Socket.o Matchmaking.o Tablero.o Texture.o game.o Partida.o
LIBS=-lpthread

%.o: %.cc $(DEPS)
	$(CC) -g -c -o $@ $< $(CFLAGS)

all: server client

server: $(OBJ) GameServer.o
	g++ -o $@ $^ $(CFLAGS) $(LIBS)

client: $(OBJ) GameClient.o
	g++ -o $@ $^ $(CFLAGS) $(LIBS)


.PHONY: clean

clean:
	rm -f *.o server client

