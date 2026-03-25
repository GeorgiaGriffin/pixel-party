#include "GameStates.h"

GameMachine::GameMachine() {
    currentState = &PlayerRegistrationState::getInstance();
    currentState->enter(this);
}

void GameMachine::setState(GameState& newState) {
    currentState->exit(this);
    currentState = &newState;
    currentState->enter(this);
}

void GameMachine::advance() {
    currentState->advance(this);
}

void GameMachine::reset() {
    // Hardware interrupt calls this — restarts everything
    setState(PlayerRegistrationState::getInstance());
}
