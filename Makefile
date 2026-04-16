CXX = g++
CXXFLAGS = -std=c++11 -Wall
OPENCV_FLAGS = `pkg-config --cflags --libs opencv4`

dice_detection: dice_detection.cpp
	$(CXX) $(CXXFLAGS) -DDICE_TEST dice_detection.cpp -o dice_detection $(OPENCV_FLAGS) -llgpio

game_state: game_state.cpp uart.cpp dice_detection.cpp
	$(CXX) $(CXXFLAGS) game_state.cpp uart.cpp dice_detection.cpp json_handling.cpp -o game $(OPENCV_FLAGS) -llgpio

init_dice_threshold: dice_init_threshold.cpp
	$(CXX) $(CXXFLAGS) dice_init_threshold.cpp -o dice_init_threshold $(OPENCV_FLAGS)

all: dice_detection game_state init_dice_threshold

clean:
	rm -f game dice_detection dice_init_threshold dice_detected.jpg dice_original.jpg dice_threshold.jpg