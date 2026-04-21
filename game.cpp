#include "game.h"

// ========= Machine =========
GameMachine::GameMachine() { setState(&regState); }

void GameMachine::setState(GameState* s) {
    state = s;
    state->enter(this);
}

void GameMachine::advance() { state->advance(this); }

void GameMachine::reset()   { setState(&regState); }

// ========= PlayerRegistration =========
void PlayerRegistrationState::enter(GameMachine* m) {
    // Actual logic:
    //   Send to Pi over UART: "REGISTER"
    //   Then constantly send over UART any changes in token sensors triggered by interrupts
    //   Does not advance until it receives over UART: "PLAYER:n"

    // Mocking with terminal inputs:
    int n = 0;
    std::cout << "Enter number of players (2-4): ";
    while (!(std::cin >> n) || n < 2 || n > 4) {
        std::cout << "Invalid. Enter 2-4: ";
        std::cin.clear();
        std::cin.ignore(1000, '\n');
    }
    m->currentPlayer = 1;
    std::cout << "[Registration] " << n << " players registered. Starting with player 1.\n";
}

void PlayerRegistrationState::advance(GameMachine* m) {
    m->setState(&m->playState);      // direct member access, no getInstance()
}

void GameplayState::advance(GameMachine* m) {
    // Actual logic:
    //   Receive over UART: "NEXT:n"  → send back "PLAYER:n"
    //   Receive over UART: "NEXT:-1" → send back "ENDGAME", transition to EndGame

    // Mock: simulate receiving NEXT:n from Pi
    int next;
    std::cout << "Pi sends NEXT: ";
    std::cin >> next;

    if (next == -1) {
        std::cout << "[UART] MCU sends Pi ENDGAME\n";
        m->setState(&m->endState);
    } else {
        std::cout << "[UART] MCU sends Pi PLAYER:" << next << "\n";
        m->currentPlayer = next;
    }
}

// ========= EndGame =========
void EndGameState::enter(GameMachine* m) {
    // Actual:
    //  Send over UART: "ENDGAME"
    //   hardware interrupt calls m->reset()

    std::cout << "Endgame state";
}