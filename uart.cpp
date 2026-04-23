#include "uart.h"
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <termios.h>
#include <vector>
#include <sstream>
#include "game_config.hpp"

// Add this line so uart.cpp knows 'config' exists in pong.cpp
extern GameConfig config;

static int uart_fd;  // shared internally

bool uart_init(const std::string& device, int baud) {
    // initialize uart stuff
    uart_fd = open(device.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
    if (uart_fd < 0) {
        perror("open");
        return false;
    }

    // fcntl(uart_fd, F_SETFL, 0); // blocking mode

RECEIVING: 0,0,0,0,0,0,0,0,0,0,0,0
RECEIVING: 0,0,0,0,0,0,0,0,0,0,0,0
    struct termios options;
    tcgetattr(uart_fd, &options);

    cfmakeraw(&options);

    cfsetispeed(&options, baud);
    cfsetospeed(&options, baud);

    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    options.c_cc[VMIN] = 1;
    options.c_cc[VTIME] = 1;

    tcsetattr(uart_fd, TCSANOW, &options);

    return true;
}


std::string uart_receive() {
    static std::string buffer;
    char c;

    while (read(uart_fd, &c, 1) > 0) {
        if (c == '\n') {
            std::string line = buffer;
            buffer.clear();

            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }

            // 👇 PUT PRINT HERE (ONLY ON COMPLETE MESSAGE)
            std::cout << "RECEIVING: " << line << std::endl;

            return line;
        } else {
            buffer += c;
        }
    }

    return "";
}


void uart_send(const std::string& msg) {
    if (msg.empty()) return;

    std::string out = msg;
    if (out.back() != '\n') {
        out += '\n';
    }
    write(uart_fd, out.c_str(), out.size());
}

void uart_close() {
    close(uart_fd);
}

void ParseUartInput(std::string line) {
    if (line.empty()) return;

    std::stringstream ss(line);
    std::string segment;
    std::vector<int> values;

    while (std::getline(ss, segment, ',')) {
        try {
            values.push_back(std::stoi(segment));
        } catch (...) { return; } 
    }

    // Index mapping for the 12-integer CSV
    if (values.size() >= 12) {
        // Player 1 (Top - Horizontal) uses J1x and B1
        config.playerOneMove     = values[0]; 
        config.playerOnePowerUp   = values[8];

        // Player 2 (Left - Vertical) uses J2y and B2
        config.playerTwoMove     = values[3]; 
        config.playerTwoPowerUp   = values[9];

        // Player 3 (Bottom - Horizontal) uses J3x and B3
        config.playerThreeMove   = values[4]; 
        config.playerThreePowerUp = values[10];

        // Player 4 (Right - Vertical) uses J4y and B4
        config.playerFourMove    = values[7]; 
        config.playerFourPowerUp  = values[11];
    }
}