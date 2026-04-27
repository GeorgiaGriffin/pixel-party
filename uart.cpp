#include "uart.h"
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <termios.h>


static int uart_fd;  // shared internally

bool uart_init(const std::string& device, int baud) {
    // initialize uart stuff
    uart_fd = open(device.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
    if (uart_fd < 0) {
        perror("open");
        return false;
    }
    std::cout << "uart init\n";

    // fcntl(uart_fd, F_SETFL, 0); // blocking mode

    struct termios options;
    tcgetattr(uart_fd, &options);

    cfmakeraw(&options);

    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);

    options.c_cflag |= (CLOCAL | CREAD);

    options.c_cc[VMIN] = 0;
    options.c_cc[VTIME] = 0;

    tcsetattr(uart_fd, TCSANOW, &options);
    tcflush(uart_fd, TCIFLUSH);

    return true;
}


std::string uart_receive() {
    static std::string buffer;
    char c;
    int n;

    // Keep reading as long as there are bytes available in the system buffer
    std::cout<<"enter function\n";
    while ((n = read(uart_fd, &c, 1)) > 0) {
        std::cout << "uart_recieve\n";
        if (c == '\n') {
            std::string line = buffer;
            buffer.clear();

            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }

            // This WILL print once a full line is built
            std::cout << "RECEIVING: " << line << std::endl;
            return line;
        } else {
            buffer += c; // Build the line character by character
        }
    }

    // static std::string buffer;  // persists across calls
    // char c;
    // int n;

    // while ((n = read(uart_fd, &c, 1)) > 0) {
    //     if (c == '\n') {
    //         std::string line = buffer;
    //         buffer.clear();

    //         if (!line.empty() && line.back() == '\r') {
    //             line.pop_back();
    //         }

    //         std::cout << "RECEIVING: " << line << std::endl;
    //         return line;  // return ONE complete message
    //     } else {
    //         buffer += c;
    //     }
    // }

    // If n is -1, it just means no more bytes are available right now.
    // We return "" so the game can draw the next frame.
    return "";
}


void uart_send(const std::string& msg) {
    std::cout << "uart send\n";
    std::cout << "SENDING: " << msg << "\n";
    if (msg.empty()) return;

    std::string out = msg;
    if (out.back() != '\n') {
        out += '\n';
    }
    write(uart_fd, out.c_str(), out.size());
}

void uart_close() {
    std::cout<<"uart close\n";
    close(uart_fd);
    std::cout<<"uart closed properly and ended the function?\n";
}
