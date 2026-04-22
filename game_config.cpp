#include "game_config.hpp"
#include "json.hpp"
#include <fstream>
#include <iostream>

using json = nlohmann::json;

bool GameConfig::read(const std::string& path) {
    std::ifstream file(path);

    if (!file.is_open()) {
        std::cerr << "Failed to open config file: " << path << "\n";
        return false;
    }

    json j;
    file >> j;

    playerOne   = j.value("playerOne", 0);
    playerTwo   = j.value("playerTwo", 0);
    playerThree = j.value("playerThree", 0);
    playerFour  = j.value("playerFour", 0);

    playerOneMove   = j.value("playerOneMove", 0);
    playerTwoMove   = j.value("playerTwoMove", 0);
    playerThreeMove = j.value("playerThreeMove", 0);
    playerFourMove  = j.value("playerFourMove", 0);

    
    playerOnePowerUp = j.value("playerOnePowerUp", 0);
    playerTwoPowerUp = j.value("playerTwoPowerUp", 0);
    playerThreePowerUp = j.value("playerThreePowerUp", 0);
    playerFourPowerUp = j.value("playerFourPowerUp", 0);

    return true;
}

bool GameConfig::write(const std::string& path) {
    json j;

    // Write all values
    j["playerOne"] = playerOne;
    j["playerTwo"] = playerTwo;
    j["playerThree"] = playerThree;
    j["playerFour"] = playerFour;

    j["playerOneMove"] = playerOneMove;
    j["playerTwoMove"] = playerTwoMove;
    j["playerThreeMove"] = playerThreeMove;
    j["playerFourMove"] = playerFourMove;

    j["playerOnePowerUp"] = playerOnePowerUp;
    j["playerTwoPowerUp"] = playerTwoPowerUp;
    j["playerThreePowerUp"] = playerThreePowerUp;
    j["playerFourPowerUp"] = playerFourPowerUp;

    j["winner"] = winner;
    

    std::ofstream file(path);

    if (!file.is_open()) {
        std::cerr << "Failed to write config file\n";
        return false;
    }

    file << j.dump(4);  // pretty print (indent = 4)

    return true;
}