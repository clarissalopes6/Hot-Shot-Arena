run:
	mkdir -p bin
	gcc -Wall -std=c99 -Iinclude src/main.c src/game.c -o bin/hotshot -lraylib -lm -lpthread -ldl -lGL -lrt -lX11
	./bin/hotshot

CC = gcc
CFLAGS = -Wall -std=c99
INCLUDES = -Iinclude
SRC = src/main.c src/game.c src/player.c
OUT = bin/hotshot

LIBS = -lraylib -lm -lpthread -ldl -lGL -lrt -lX11

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) $(INCLUDES) $(SRC) -o $(OUT) $(LIBS)

run: all
	./$(OUT)

clean:
	rm -f $(OUT)
