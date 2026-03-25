#pragma once
#include <iostream>

// Forward declaration
class GameMachine;

// ─────────────────────────────────────────
// Abstract base
// ─────────────────────────────────────────
class GameState {
public:
    virtual void enter(GameMachine* machine) = 0;
    virtual void advance(GameMachine* machine) = 0;
    virtual void exit(GameMachine* machine) = 0;
    virtual ~GameState() = default;
};

// ─────────────────────────────────────────
// PlayerRegistrationState
// ─────────────────────────────────────────
class PlayerRegistrationState : public GameState {
public:
    static PlayerRegistrationState& getInstance();
    void enter(GameMachine* machine) override;
    void advance(GameMachine* machine) override;
    void exit(GameMachine* machine) override;
private:
    PlayerRegistrationState() = default;
};

// ─────────────────────────────────────────
// GameplayState
// ─────────────────────────────────────────
class GameplayState : public GameState {
public:
    static GameplayState& getInstance();
    void enter(GameMachine* machine) override;
    void advance(GameMachine* machine) override;
    void exit(GameMachine* machine) override;
private:
    GameplayState() = default;
};

// ─────────────────────────────────────────
// EndGameState
// ─────────────────────────────────────────
class EndGameState : public GameState {
public:
    static EndGameState& getInstance();
    void enter(GameMachine* machine) override;
    void advance(GameMachine* machine) override;
    void exit(GameMachine* machine) override;
private:
    EndGameState() = default;
};

// ─────────────────────────────────────────
// GameMachine
// ─────────────────────────────────────────
class GameMachine {
public:
    GameMachine();
    GameState* getCurrentState() const { return currentState; }
    void setState(GameState& newState);
    void advance();
    void reset();
    int getCurrentPlayer() const { return currentPlayer; }
    void setCurrentPlayer(int player) { currentPlayer = player; }
private:
    GameState* currentState;
    int currentPlayer;
};