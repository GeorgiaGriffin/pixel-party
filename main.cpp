#include "game.h"

int main() {
    GameMachine game;          // boots into PlayerRegistration::enter()
    game.advance();            // Gameplay

    while (game.getState() == &game.playState)
        game.advance();        // loops turns until EndGame

    return 0;
}
// g++ main.cpp game.cpp -o game