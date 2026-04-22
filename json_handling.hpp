#pragma once
#include <string>

// Represents one player
struct Player {
    int score = 0;
    int location = 0;
    int out = 0;
    int active = 0;
};

class State {
public:
    static const int NUM_PLAYERS = 4;

    Player players[NUM_PLAYERS];

    int start = 0;
    int state = 0;
    int totalActive = 0;
    int winner = 0;
    int winnerScore = 0;
    int minigameWinner = 0;

    bool read(const std::string& path);
    bool readMini(const std::string& path);
    bool write(const std::string& path);
    bool writeMini(const std::string& path);

    void updateActivePlayers();
    void resetData();
    bool checkPlayerComplete(int player);
};