#include "game.h"
#include "hardware.h"
#include <stdio.h>
#include <stdlib.h>

// ========= Machine =========
GameMachine* g_machine = nullptr;
GameMachine::GameMachine() {}

void GameMachine::setState(GameState* s) {
    state = s;
    state->enter(this);
}

void GameMachine::advance() { state->advance(this); }

void GameMachine::reset()   { setState(&regState); }

static bool parseNext(const char* buf, int* out) {
    if (buf[0]=='N' && buf[1]=='E' && buf[2]=='X' && buf[3]=='T' && buf[4]==':') {
        *out = atoi(&buf[5]);
        return true;
    }
    return false;
}

void handleTokenEvents(GameMachine* m) {
    uint8_t events = m->tokenEvent;
    // Clear all pending events at once
    m->tokenEvent = 0;

    for (int i = 0; i < 4; i++) {
        if (events & (1 << i)) {
            printf("TOKEN%d:%d\r\n", i + 1, m->tokenState[i]);
        }
    }
}

// ========= PlayerRegistration =========
void PlayerRegistrationState::enter(GameMachine* m) {
    m->registrationReady = true;
    m->startPressed = false;

    printf("REGISTER\r\n");
}

void PlayerRegistrationState::advance(GameMachine* m) {
    if (m->startPressed) {
        m->startPressed = false; 
        printf("START\r\n"); // This will now print every time you click start
    }
    // Only transition if the Pi has sent a command to begin the game
    // We check the UART buffer while still in the Registration State
    if (USART6->SR & USART_SR_RXNE) {
        char buf[32];
        int n;
        USART6_ReadLine(buf, sizeof(buf));
        
        if (parseNext(buf, &n) && n != -1) {
            m->currentPlayer = n;
            printf("PLAYER:%d\r\n", n); // Sync with Pi
            m->setState(&m->playState); // NOW move to gameplay
        }
    }
}

void GameplayState::enter(GameMachine* m) {
    printf("[Gameplay] Waiting for NEXT:n...\r\n");
}

void GameplayState::advance(GameMachine* m) {
    // ONLY read if data is actually available
    if (!(USART6->SR & USART_SR_RXNE)) {
        return;
    }

    char buf[32];
    int n;
    USART6_ReadLine(buf, sizeof(buf));

    if (parseNext(buf, &n)) {
        if (n == -1) {
            printf("ENDGAME\r\n"); // Send to Pi
            m->setState(&m->endState);
        } else {
            m->currentPlayer = n;
            printf("PLAYER:%d\r\n", n); // Send to Pi
        }
    } else {
        // This catches "false_start" or other messages from Pi
        m->currentPlayer = n;
        printf("[Gameplay] Ignoring: %s\r\n", buf);
    }
    
}

// ========= EndGame =========
void EndGameState::enter(GameMachine* m) {
    // Actual:
    //  Send over UART: "ENDGAME"
    //   hardware interrupt calls m->reset()
    printf("ENDGAME\r\n");
}