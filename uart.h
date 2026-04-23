#pragma once
#include <string>
#include <termios.h>

bool uart_init(const std::string& device, int baud);
void uart_send(const std::string& msg);
std::string uart_receive();
void uart_close();
