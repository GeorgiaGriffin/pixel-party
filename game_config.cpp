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
    playerTwo   = j.value("playerTwo", 1);
    playerThree = j.value("playerThree", 0);
    playerFour  = j.value("playerFour", 1);

    playerOneMove   = j.value("playerOneMove", 0);
    playerTwoMove   = j.value("playerTwoMove", 0);
    playerThreeMove = j.value("playerThreeMove", 0);
    playerFourMove  = j.value("playerFourMove", 0);

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

    std::ofstream file(path);

    if (!file.is_open()) {
        std::cerr << "Failed to write config file\n";
        return false;
    }

    file << j.dump(4);  // pretty print (indent = 4)

    return true;
}