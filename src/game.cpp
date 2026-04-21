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

// ========= PlayerRegistration =========
void PlayerRegistrationState::enter(GameMachine* m) {
    m->registrationReady = false;
    printf("\n[Registration] Waiting for players to place tokens.\r\n");
    printf("\n[Registration] Press START when ready.\r\n");

    // Block here waiting for Pi to send NEXT:n
    // Real logic: parse "NEXT:n" over UART
    // Mock: type the first player number into serial monitor
    
    // Flush any garbage in the receive buffer
    while (USART6->SR & USART_SR_RXNE) {
        (void)USART6->DR;
    }

    printf("Waiting for NEXT:n from Pi: \r\n");
    int n = USART6_ReadInt();
    m->currentPlayer = n;
    m->registrationReady = true;
    printf("\n[Registration] Ready. First player: %d\r\n", n);
}

void PlayerRegistrationState::advance(GameMachine* m) {
    if (!m->registrationReady) {
        printf("\n[Registration] Not ready yet — waiting for NEXT:n.\r\n");
        return;
    }
    m->setState(&m->playState);      // direct member access, no getInstance()
}

void GameplayState::advance(GameMachine* m) {
    // Actual logic:
    //   Receive over UART: "NEXT:n"  → send back "PLAYER:n"
    //   Receive over UART: "NEXT:-1" → send back "ENDGAME", transition to EndGame

    // Mock: simulate receiving NEXT:n from Pi
    printf("Pi sends NEXT: \r\n");
    int next = USART6_ReadInt();

    if (next == -1) {
        printf("[UART] MCU sends Pi: ENDGAME\r\n");
        m->setState(&m->endState);
    } else {
        printf("[UART] MCU sends Pi: PLAYER:%d\r\n", next);
        m->currentPlayer = next;
    }
}

// ========= EndGame =========
void EndGameState::enter(GameMachine* m) {
    // Actual:
    //  Send over UART: "ENDGAME"
    //   hardware interrupt calls m->reset()
    printf("[EndGame] Game over.\r\n");
}