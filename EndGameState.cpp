#include "GameStates.h"
#include <iostream>

EndGameState& EndGameState::getInstance() {
    static EndGameState instance;
    return instance;
}

void EndGameState::enter(GameMachine* machine) {
    std::cout << "[EndGame] Entering: game over. Displaying results.\n";
    // call to display the end game graphics
    // It will sit here until the game restarts with hardware interrupts
}

void EndGameState::advance(GameMachine* machine) {
    // Does nothing. Just here to fit base class
}

void EndGameState::exit(GameMachine* machine) {
    // Does nothing. Just here to fit base class
}