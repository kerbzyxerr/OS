CC = gcc
CFLAGS = -Wall -Wextra -I./include
SRC_DIR = src
BIN_DIR = bin

.PHONY: all clean directories

all: directories $(BIN_DIR)/main

directories:
	mkdir -p $(BIN_DIR)

$(BIN_DIR)/main: main.c $(SRC_DIR)/parent.c $(SRC_DIR)/child.c
	$(CC) $(CFLAGS) -o $(BIN_DIR)/child $(SRC_DIR)/child.c
	$(CC) $(CFLAGS) -o $@ main.c $(SRC_DIR)/parent.c

clean:
	rm -rf $(BIN_DIR)