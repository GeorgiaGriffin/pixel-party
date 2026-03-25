#include "GameStates.h"
#include <iostream>

GameplayState& GameplayState::getInstance() {
    static GameplayState instance;
    return instance;
}

// Stub: in reality, sends "START_TURN playerN" to Pi and waits for response
// Returns next player index, or -1 if game over
int runPlayerTurn(int currentPlayer) {
    std::cout << "[Turn] Player " << currentPlayer << "'s turn.\n";
    std::cout << "[Turn] Simulating: dice roll, position update, tile action...\n";

    int next;
    std::cout << "[Turn] Enter next player (or -1 for game over): ";
    std::cin >> next;
    return next;
}

// =============== Gameplay State Control ===============

void GameplayState::enter(GameMachine* machine) {
    std::cout << "[Gameplay] Entering: starting player turns.\n";
    // Call graphics to switch to board
}

void GameplayState::advance(GameMachine* machine) {
    // Start the player turn logic
    // call PlayerTurn(currentplayernumber)
    // PlayerTurn will run on the Pi
    // PlayerTurn will either return endgame or it will return the next player

    int result = runPlayerTurn(machine->getCurrentPlayer());
    std::cout << "[Gameplay] Player turn complete. Checking end condition...\n";

    // Check if end-game condition is met
    if (result == -1) {
        std::cout << "[Gameplay] Game over condition met.\n";
        machine->setState(EndGameState::getInstance());
    } else {
        std::cout << "[Gameplay] Next player: " << result << "\n";
        machine->setCurrentPlayer(result);
    }

}

void GameplayState::exit(GameMachine* machine) {
    std::cout << "[Gameplay] Exiting: all turns done.\n";
    // Probably won't be adding anymore logic here
    // Exit will just go to the endgame state, and that will handle stuff
}
