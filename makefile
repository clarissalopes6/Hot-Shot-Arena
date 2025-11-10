CC = gcc
SRC = src/main.c
OUT_DIR = bin
OUT = $(OUT_DIR)/hotshot

PKG_CFLAGS := $(shell pkg-config --cflags raylib 2>/dev/null)
PKG_LIBS   := $(shell pkg-config --libs raylib 2>/dev/null)

ifeq ($(PKG_LIBS),)
LIBS = -lraylib -lm -lpthread -ldl -lGL -lrt -lX11
CFLAGS =
else
LIBS = $(PKG_LIBS)
CFLAGS = $(PKG_CFLAGS)
endif

all: $(OUT_DIR) $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(OUT_DIR):
	mkdir -p $(OUT_DIR)

run: all
	./$(OUT)

clean:
	rm -rf $(OUT_DIR)