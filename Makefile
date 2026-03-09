CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -g

SRC = src/main.c src/server.c src/request.c src/response.c src/router.c
OBJ = $(SRC:.c=.o)
TARGET = http_server

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

run: all
	./$(TARGET)

.PHONY: all clean run