#include "uart.h"
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <termios.h>
#include <sys/ioctl.h>


static int uart_fd;  // shared internally

bool uart_init(const std::string& device, int baud) {
    // initialize uart stuff
    uart_fd = open(device.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
    if (uart_fd < 0) {
        perror("open");
        return false;
    }

    // fcntl(uart_fd, F_SETFL, 0); // blocking mode

    // Prevent FTDI from toggling DTR/RTS on open (stops garbage bytes to MCU)
    int flags = TIOCM_DTR | TIOCM_RTS;
    ioctl(uart_fd, TIOCMBIC, &flags);

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
    // Clear the output buffer before anything gets sent
    tcflush(uart_fd, TCIOFLUSH);  // flush both input AND output

    return true;
}


std::string uart_receive() {
    static std::string buffer;
    char c;
    int n;

    // Keep reading as long as there are bytes available in the system buffer
    while ((n = read(uart_fd, &c, 1)) > 0) {
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

    // If n is -1, it just means no more bytes are available right now.
    // We return "" so the game can draw the next frame.
    return "";
}


void uart_send(const std::string& msg) {
    std::cout << "SENDING: " << msg << "\n";
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
