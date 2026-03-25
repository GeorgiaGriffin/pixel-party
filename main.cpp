#include "GameStates.h"
#include <iostream>

int main() {
    std::cout << "=== Device power on ===\n";
    GameMachine game; // starts in PlayerRegistration automatically

    game.advance(); // PlayerRegistrationState::advance — transition to gameplay

    // loop until we leave gameplay
    while (game.getCurrentState() == &GameplayState::getInstance()) {
        game.advance();
    }

    return 0;
}

// compile with
//  g++ main.cpp PlayerRegistrationState.cpp GameplayState.cpp EndGameState.cpp GameMachine.cpp -o game
