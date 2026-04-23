#include "game.h"
#include "hardware.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

static bool isCommand(const char* buf, const char* cmd) {
    return (strcmp(buf, cmd) == 0);
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

void MinigameState::enter(GameMachine* m) {
    // nothing happens?
}

void MinigameState::advance(GameMachine* m) {
    // === STEP A: Clear Overrun/Noise/Frame errors ===
    // This must happen BEFORE checking RXNE
    if (USART6->SR & (USART_SR_ORE | USART_SR_NE | USART_SR_FE)) {
        volatile uint32_t dummy = USART6->SR; 
        dummy = USART6->DR; // Reading SR then DR in sequence clears these flags
    }

    // 1. ALWAYS check for the STOP command first
    if (USART6->SR & USART_SR_RXNE) {
        char buf[32];
        USART6_ReadLine(buf, sizeof(buf));
        if (strstr(buf, "MINIGAME_STOP") != nullptr) {
            m->setState(&m->playState);
            return;
        }
    }

    // 2. Use get_tick() to throttle the output (Non-blocking)
    static uint32_t last_send_time = 0;
    uint32_t current_time = get_tick();

    if (current_time - last_send_time >= 50) { // Send data every 50ms
        last_send_time = current_time;

        // Read joystick and button data
        for(int i = 0; i < 8; i++) {
            uint16_t raw_val = joystick_data[i];
            if (raw_val >= 2000 && raw_val <= 2100) m->last_joystick[i] = 0;
            else if (raw_val > 3000) m->last_joystick[i] = 1;
            else if (raw_val < 1000) m->last_joystick[i] = -1;
        }

        for(int i = 0; i < 5; i++) {
            m->last_buttons[i] = button_state[i];
        }

        // Send data only once every 50ms
        printf("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
            m->last_joystick[0], m->last_joystick[1], m->last_joystick[2], m->last_joystick[3], 
            m->last_joystick[4], m->last_joystick[5], m->last_joystick[6], m->last_joystick[7],
            m->last_buttons[1], m->last_buttons[2], m->last_buttons[3], m->last_buttons[4]);
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
    int n = 0;
    USART6_ReadLine(buf, sizeof(buf));

    if (parseNext(buf, &n)) {
        if (n == -1) {
            printf("ENDGAME\r\n"); // Send to Pi
            m->setState(&m->endState);
        } else {
            m->currentPlayer = n;
            printf("PLAYER:%d\r\n", n); // Send to Pi
        }
    }
    else if (isCommand(buf, "MINIGAME")) {
        m->setState(&m->miniState);
    }
    else {
        // This catches "false_start" or other messages from Pi
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