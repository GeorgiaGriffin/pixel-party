CXX = g++
CXXFLAGS = -std=c++11 -Wall
OPENCV_FLAGS = `pkg-config --cflags --libs opencv4`

dice_detection: dice_detection.cpp
	$(CXX) $(CXXFLAGS) dice_detection.cpp -o dice_detection $(OPENCV_FLAGS) -llgpio

clean:
	rm -f dice_detection
