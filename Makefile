CXX = g++

CXXFLAGS = -O1 -Wall -std=c++17

SRC = pong.cpp game_config.cpp
TARGET = pong.exe

all:
	$(CXX) $(SRC) -o $(TARGET) $(CXXFLAGS) \
	-lraylib -lopengl32 -lgdi32 -lwinmm

clean:
	rm -f $(TARGET)