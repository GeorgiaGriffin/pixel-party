CXX = g++
CXXFLAGS = -std=c++11 -Wall
OPENCV_FLAGS = `pkg-config --cflags --libs opencv4`

dice_detection: dice_detection.cpp
	$(CXX) $(CXXFLAGS) dice_detection.cpp -o dice_detection $(OPENCV_FLAGS) -llgpio

game_state: game_state.cpp uart.cpp
	$(CXX) $(CXXFLAGS) game_state.cpp uart.cpp -o game

all: dice_detection game_state

clean:
	rm -f dice_detection dice_detected.jpg dice_original.jpg dice_threshold.jpg
