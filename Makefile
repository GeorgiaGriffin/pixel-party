# Define paths
CC = C:/raylib/w64devkit/bin/gcc.exe
RAYLIB_PATH = C:/raylib/raylib/src

# Compiler flags
CFLAGS = -O1 -Wall -std=c99 -Wno-missing-braces
INCLUDES = -I$(RAYLIB_PATH)
LIBS = -L$(RAYLIB_PATH) -lraylib -lopengl32 -lgdi32 -lwinmm

# Fix for "cannot execute as" - we must append the bin path to the system PATH
export PATH := C:/raylib/w64devkit/bin:$(PATH)

all:
	$(CC) pong.c -o pong.exe $(CFLAGS) $(INCLUDES) $(LIBS)
	@echo Build successful! Run with ./pong.exe

clean:
	del pong.exe