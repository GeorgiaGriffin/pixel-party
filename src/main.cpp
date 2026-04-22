#include "hardware.h"
#include "game.h"
#include <stdio.h>

extern "C" void __libc_init_array(void);  // C++ global constructors

int main(void) {
    __libc_init_array();  // run C++ constructors before anything else

    GPIO_Init();
    USART6_Init();
    ADC_Init();
    EXTI_Init();

    // delay so serial monitor connects
    for (volatile int i = 0; i < 4000000; i++);

    GameMachine game;
    g_machine = &game;
    game.setState(&game.regState);

    while (1) {
        handleTokenEvents(&game);
        game.advance();
    }
}