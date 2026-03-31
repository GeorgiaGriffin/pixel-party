#include "game_state.h"
#include "uart.h"
#include <iostream>

static void process_message(const std::string& msg);
static void handle_registration();
static void handle_turn(int player);
static void handle_endgame();


void game_loop() {
    while (true) {
        std::string msg = uart_receive();
        if (!msg.empty()) {
            process_message(msg);
        }
    }
}


static void process_message(const std::string& msg) {
    if (msg == "REGISTER\n") {
        handle_registration();
    }
    else if (msg.find("PLAYER:") == 0) {
        int player = std::stoi(msg.substr(7));
        handle_turn(player);
    }
    else if (msg == "ENDGAME\n") {
        handle_endgame();
    }
    else {
        uart_send("ERROR\n");
    }
}


static void handle_registration() {
    std::cout << "Enter Player Registration\n";

    // initialize all variables
    // call the graphics to start registration
    // after registration confirmed, then get the first player
    int firstPlayer = 1; // replace later

    uart_send("PLAYER:" + std::to_string(firstPlayer) + "\n");
}


static void handle_turn(int player) {
    std::cout << "Player Turn for player " << player << "\n";
    // Dice detection
    // Move player with dice 
    // Is position > end? Player complete
    // Else do tile action

    // Get next player or end of game
    int result = player + 1;
    if (result > 4) result = 1;

    uart_send("NEXT:" + std::to_string(result) + "\n");
}


static void handle_endgame() {
    std::cout << "Endgame\n";
    // Sort active players by score
    // Call graphics for end game
}


int main() {
    uart_init("/dev/serial0");
    game_loop();
    return 0;
}
