#include "uart.h"
#include <unistd.h>
#include <fcntl.h>
#include <iostream>

static int uart_fd;  // shared internally

bool uart_init(const std::string& device) {
    // initialize uart stuff 

    // this is from chat:
    // uart_fd = open(device.c_str(), O_RDWR | O_NOCTTY);
    // configure termios here
    // return uart_fd >= 0;

    // testing
    std::cout << "Commands to be received from the MCU:\n";
    std::cout << "  REGISTER\n";
    std::cout << "  PLAYER:int\n";
    std::cout << "  ENDGAME\n\n";
    return true;
}

std::string uart_receive() {
    // from chat:
    // char buffer[128];
    // int n = read(uart_fd, buffer, sizeof(buffer));
    // if (n > 0) {
    //     return std::string(buffer, n);
    // }
    // return "";

    // testing:
    fd_set set;
    struct timeval timeout;

    FD_ZERO(&set);
    FD_SET(STDIN_FILENO, &set);

    timeout.tv_sec = 0;
    timeout.tv_usec = 100000; // 100ms

    int rv = select(STDIN_FILENO + 1, &set, NULL, NULL, &timeout);

    if (rv > 0) {
        std::string input;
        std::getline(std::cin, input);
        if (!input.empty()) {
            return input + "\n";
        }
    }

    return "";

}

void uart_send(const std::string& msg) {
    // from chat:
    //write(uart_fd, msg.c_str(), msg.size());

    // testing:
    std::cout << "UART send: " << msg.c_str() << "\n";
}
