#pragma once
#include "stm32f4xx.h"

class GameMachine;
extern GameMachine* g_machine;  // global so ISRs can reach it

//========= Function declarations =========
void handleTokenEvents(GameMachine* m);

// ========= Abstract base =========
class GameState {
public:
    virtual void enter(GameMachine* m) = 0;
    virtual void advance(GameMachine* m) = 0;
    virtual ~GameState() = default;
};

// ========= States =========
class MinigameState : public GameState {
public:
    void enter(GameMachine* m) override;
    void advance(GameMachine* m) override;
};

class PlayerRegistrationState : public GameState {
public:
    void enter(GameMachine* m) override;
    void advance(GameMachine* m) override;
};

class GameplayState : public GameState {
public:
    void enter(GameMachine* m) override;
    void advance(GameMachine* m) override;
};

class EndGameState : public GameState {
public:
    void enter(GameMachine* m) override;
    void advance(GameMachine* m) override {}  // sits here until reset()
};

// ========= Machine =========
class GameMachine {
public:
    GameMachine();
    void advance();
    void reset();
    void setState(GameState* s);
    GameState* getState() const { return state; }

    int currentPlayer = 1;
    bool registrationReady = false;
    bool startPressed = false;
    volatile uint8_t tokenEvent = 0;
    volatile uint8_t tokenState[4] = {0};

    int8_t last_joystick[8] = {0};
    uint8_t last_buttons[5] = {0};

    // States are public so state classes and main can reference them
    MinigameState miniState;
    PlayerRegistrationState regState;
    GameplayState           playState;
    EndGameState            endState;

private:
    GameState* state;
};