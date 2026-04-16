#include "game_state.h"
#include "uart.h"
#include "dice_detection.h"
#include "json_handling.hpp"
#include <iostream>

static void process_message(const std::string& msg);
static void handle_registration();
static void handle_turn(int player);
static void handle_endgame();

static State gameState;

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

    // call the graphics to start registration
    system("./graphics &");

    // every time a player removes token, write to json
    
    while (true) {
        // testing without button:
        std::string input;
        while (true) {
            std::cout << "Start game? Type 'yes': ";
            std::getline(std::cin, input);
            if (input == "yes") {
                break;
            }
        }

        // on start button, mark any out players as active
        gameState.read("state.json");
        gameState.start = 1;
        gameState.updateActivePlayers();
        gameState.write("state.json");

        // count active players
        int activeCount = 0;
        for (int i = 0; i < State::NUM_PLAYERS; i++) {
            if (gameState.players[i].active == 1) {
                activeCount++;
            }
        }

        if (activeCount < 2) {
            std::cout << "Need at least 2 active players. Restarting registration...\n";
            //gameState.start = 0;
            gameState.write("state.json");
            continue;
        }

        break; // valid game start
    }


    // then write to json to toggle the start value 1 0
    gameState.read("state.json");
    gameState.start = 0;
    gameState.write("state.json");

    // determine first player based on the json values
    int firstPlayer;
    for (int i = 0; i < State::NUM_PLAYERS; i++) {
        if (gameState.players[i].active == 1) {
            firstPlayer = i + 1; // convert index (0–3) to player number (1–4)
            break;
        }
    }

    uart_send("PLAYER:" + std::to_string(firstPlayer) + "\n");
}


static void handle_turn(int player) {
    std::cout << "Player Turn for player " << player << "\n";
    // Dice detection
    int dice_val = runDiceDetection();
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
