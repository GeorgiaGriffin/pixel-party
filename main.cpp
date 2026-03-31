#include "uart.h"
#include "game_state.h"

int main() {
    uart_init("/dev/serial0");
    game_loop();
    return 0;
}
