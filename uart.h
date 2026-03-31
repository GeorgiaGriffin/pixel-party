#pragma once
#include <string>

bool uart_init(const std::string& device);
std::string uart_receive();
void uart_send(const std::string& msg);
