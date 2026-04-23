CXX = g++
CXXFLAGS = -O1 -Wall -std=c++17

SRC = pong.cpp game_config.cpp uart.cpp
OUT = pong

LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

all:
	$(CXX) $(SRC) -o $(OUT) $(CXXFLAGS) $(LIBS) 

