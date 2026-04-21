CXX = g++
CXXFLAGS = -g -Wall

TARGET = game
SRCS = main.cpp game.cpp

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)