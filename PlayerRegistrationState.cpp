#include "GameStates.h"
#include <iostream>

PlayerRegistrationState& PlayerRegistrationState::getInstance() {
    static PlayerRegistrationState instance;
    return instance;
}

// Communicates with the Pi for registration
// Returns the index of the first player
// DUMMY: right now just takes keyboard input and returns 1
int registerPlayers() {
    int numPlayers = 0;
    while (numPlayers < 2 || numPlayers > 4) {
        std::cout << "Enter number of players (2-4): ";
        std::cin >> numPlayers;
    }
    return 1; // first player's turn
}


// =============== Player Registration State Control ===============

void PlayerRegistrationState::enter(GameMachine* machine) {
    std::cout << "[PlayerRegistration] Entering: Player Registration.\n";
    // call a function that won't return until the button is pressed to confirm registration
    int firstPlayer = registerPlayers();
    machine->setCurrentPlayer(firstPlayer);
}

void PlayerRegistrationState::advance(GameMachine* machine) {
    machine->setState(GameplayState::getInstance());
}

void PlayerRegistrationState::exit(GameMachine* machine) {
    std::cout << "[PlayerRegistration] Exiting.\n";
}