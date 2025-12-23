CC = gcc
CFLAGS = -I./include -Wall -Wextra -O2 -pthread -std=c11 -D_POSIX_C_SOURCE=200112L
LDFLAGS = -pthread -lm

TARGET = montecarlo
SRCS = main.c src/montecarlo.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

debug: CFLAGS += -g -fsanitize=thread
debug: LDFLAGS += -fsanitize=thread
debug: clean $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

run: all
	./$(TARGET) 4 1000000 1.0

.PHONY: all clean run debug