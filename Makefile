CXX = g++
CXXFLAGS = -std=c++11 -Wall
OPENCV_FLAGS = `pkg-config --cflags --libs opencv4`

dice_detector: dice_detector.cpp
	$(CXX) $(CXXFLAGS) dice_detector.cpp -o dice_detector $(OPENCV_FLAGS)

clean:
	rm -f dice_detector
