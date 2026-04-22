#pragma once
#include <string>

struct GameConfig {
    int playerOne = 0;
    int playerTwo = 0;
    int playerThree = 0;
    int playerFour = 0;
    int playerOneMove = 0;
    int playerTwoMove = 0;
    int playerThreeMove = 0;
    int playerFourMove = 0;
    int playerOnePowerUp = 0;
    int playerTwoPowerUp = 0;
    int playerThreePowerUp = 0;
    int playerFourPowerUp = 0;
    
    bool read(const std::string& path);
    bool write(const std::string& path);
};