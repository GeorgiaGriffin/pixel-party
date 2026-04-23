CXX = g++

# Combine flags (safe superset)
CXXFLAGS = -Wall -std=c++17

# OpenCV (dice project)
OPENCV_FLAGS = `pkg-config --cflags --libs opencv4`

# Raylib (pong project)
RAYLIB_LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# ---------- Dice / Game Targets ----------

dice_detection: dice_detection.cpp
	$(CXX) $(CXXFLAGS) -DDICE_TEST dice_detection.cpp -o dice_detection $(OPENCV_FLAGS) -llgpio

game_state: game_state.cpp uart.cpp dice_detection.cpp json_handling.cpp
	$(CXX) $(CXXFLAGS) game_state.cpp uart.cpp dice_detection.cpp json_handling.cpp -o game $(OPENCV_FLAGS) -llgpio

init_dice_threshold: dice_init_threshold.cpp
	$(CXX) $(CXXFLAGS) dice_init_threshold.cpp -o dice_init_threshold $(OPENCV_FLAGS)

# ---------- Pong Target ----------

pong: pong.cpp game_config.cpp
	$(CXX) pong.cpp game_config.cpp -o pong $(CXXFLAGS) $(RAYLIB_LIBS)

# ---------- Combined ----------

all: dice_detection game_state init_dice_threshold pong

clean:
	rm -f game dice_detection dice_init_threshold pong \
	dice_detected.jpg dice_original.jpg dice_threshold.jpg
