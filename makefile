CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude

SRC = src/main.c src/menu.c src/game.c src/assets.c src/list.c
OUT = bin/hotshot

LIBS = -lraylib -lm -lpthread -ldl -lGL -lrt -lX11

all:
	mkdir -p bin
	$(CC) $(CFLAGS) $(SRC) -o $(OUT) $(LIBS)

run: all
	./bin/hotshot
