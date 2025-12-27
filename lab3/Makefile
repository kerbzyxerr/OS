CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200809L
LDFLAGS = -lrt -lpthread

SRC_DIR = src
INCLUDE_DIR = include
BIN_DIR = bin
OBJ_DIR = obj

SERVER_SOURCES = $(SRC_DIR)/main.c $(SRC_DIR)/server.c
SERVER_OBJECTS = $(OBJ_DIR)/main.o $(OBJ_DIR)/server.o

CLIENT_SOURCES = $(SRC_DIR)/client.c
CLIENT_OBJECTS = $(OBJ_DIR)/client.o

all: directories $(BIN_DIR)/server $(BIN_DIR)/client

directories:
	mkdir -p $(BIN_DIR) $(OBJ_DIR)

$(BIN_DIR)/server: $(SERVER_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(BIN_DIR)/client: $(CLIENT_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/main.o: $(SRC_DIR)/main.c $(INCLUDE_DIR)/server.h
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(OBJ_DIR)/server.o: $(SRC_DIR)/server.c $(INCLUDE_DIR)/server.h
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(OBJ_DIR)/client.o: $(SRC_DIR)/client.c $(INCLUDE_DIR)/client.h
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)

.PHONY: all clean directories