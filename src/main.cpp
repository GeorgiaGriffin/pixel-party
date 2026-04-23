#include "hardware.h"
#include "game.h"
#include <stdio.h>
#include <cstdlib>

extern "C" void __libc_init_array(void);  // C++ global constructors

bool changed = false;

// 2. Deadband threshold to ignore minor ADC jitter (e.g., +/- 10 counts)
const uint16_t ADC_NOISE_THRESHOLD = 100;

GameMachine game;

int main(void) {
    __libc_init_array();  // run C++ constructors before anything else

    GPIO_Init();
    USART6_Init();
    dma_setup();
    ADC_Init();
    EXTI_Init();

    SysTick_Config(SystemCoreClock / 1000);

    // delay so serial monitor connects
    for (volatile int i = 0; i < 4000000; i++);

    g_machine = &game;
    game.setState(&game.regState);

    while (1) {
        handleTokenEvents(&game);
        game.advance();

        // ========== minigame inputs ==========
    //     changed = true;

    //     // Check for Joystick changes
    //     for(int i = 0; i < 8; i++) {
    //         int8_t current_state = 0;
    //         uint16_t raw_val = joystick_data[i];

    //         // Corrected Comparison Logic
    //         if (raw_val >= 2000 && raw_val <= 2100) { current_state = 0;  } // Deadzone
    //         else if (raw_val > 3000) { current_state = 1; }  // Pushed Positive 
    //         else if (raw_val < 1000) { current_state = -1; } // Pushed Negative
    //         else { current_state = last_joystick[i]; }   // This catches the areas between the thresholds to prevent flickering

    //         // Check if THIS specific index changed
    //         if (current_state != last_joystick[i]) {
    //             changed = true;
    //             last_joystick[i] = current_state;
    //         }
    //     }

    //     // Check for Button/Token changes
    //     for(int i = 0; i < 5; i++) {
    //         if (button_state[i] != last_buttons[i]) {
    //             changed = true;
    //             last_buttons[i] = button_state[i];
    //         }
    //     }

    //     // 3. Print only if something moved
    //     if (changed) {
    //         printf("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
    //             last_joystick[0], last_joystick[1], last_joystick[2], last_joystick[3], 
    //             last_joystick[4], last_joystick[5], last_joystick[6], last_joystick[7],
    //             last_buttons[1], last_buttons[2], last_buttons[3], last_buttons[4]);
    //     }
    // ============ minigame =====================
    }
}