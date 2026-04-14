# Compiler
CC = gcc
CFLAGS = -O1 -Wall -std=c99

# Use pkg-config to find raylib headers/libs
RAYLIB_FLAGS = $(shell pkg-config --cflags --libs raylib)

# Target
TARGET = pong.exe
SRC = pong.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS) $(RAYLIB_FLAGS)
	@echo "Build successful! Run with ./$(TARGET)"

clean:
	rm -f $(TARGET)