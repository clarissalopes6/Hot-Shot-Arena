CC = gcc
CFLAGS = -Wall -std=c99 -Iinclude
SRC = $(shell ls src/*.c)
OUT = bin/hotshot
LIBS = -lraylib -lm -lpthread -ldl -lGL -lrt -lX11

all:
	mkdir -p bin
	$(CC) $(CFLAGS) $(SRC) -o $(OUT) $(LIBS)

run: all
	./$(OUT)

clean:
	rm -rf bin
