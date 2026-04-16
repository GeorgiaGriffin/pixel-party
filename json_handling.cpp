#include "json_handling.hpp"
#include "external/json.hpp"
#include <fstream>
#include <iostream>

using json = nlohmann::json;

bool State::read(const std::string& path) {

    std::ifstream file(path);

    if (!file.is_open()) {
        std::cerr << "Failed to open state file: " << path << "\n";
        return false;
    }

    json j;

    try {
        file >> j;
    } catch (...) {
        std::cerr << "Invalid JSON format\n";
        return false;
    }

    // Loop through players
    for (int i = 0; i < NUM_PLAYERS; i++) {
        std::string idx = std::to_string(i + 1);

        players[i].score    = j.value("Player" + idx + "Score", 0);
        players[i].location = j.value("player" + idx + "Location", 0);
        players[i].out      = j.value("player" + idx + "Out", 0);
        players[i].active   = j.value("player" + idx + "active", 0);
    }

    // Game-wide values
    start       = j.value("start", 0);
    state       = j.value("state", 0);
    totalActive = j.value("totalActive", 0);
    winner      = j.value("winner", 0);
    winnerScore = j.value("winnerScore", 0);

    return true;
}


bool State::write(const std::string& path) {

    json j;

    for (int i = 0; i < NUM_PLAYERS; i++) {
        std::string idx = std::to_string(i + 1);

        j["Player" + idx + "Score"]     = players[i].score;
        j["player" + idx + "Location"]  = players[i].location;
        j["player" + idx + "Out"]       = players[i].out;
        j["player" + idx + "active"]    = players[i].active;
    }

    j["start"]       = start;
    j["state"]       = state;
    j["totalActive"] = totalActive;
    j["winner"]      = winner;
    j["winnerScore"] = winnerScore;

    // WRITE TO TEMP FILE FIRST
    std::string tempPath = path + ".tmp";

    std::ofstream file(tempPath);
    if (!file.is_open()) {
        std::cerr << "Failed to write temp state file\n";
        return false;
    }

    file << j.dump(4);
    file.close();

    // ATOMIC REPLACE
    std::rename(tempPath.c_str(), path.c_str());

    return true;
}


void State::updateActivePlayers() {
    for (int i = 0; i < NUM_PLAYERS; i++) {
        players[i].active = (players[i].out == 1) ? 1 : 0;
    }
}
