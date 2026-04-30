#include "game_state.h"
#include "uart.h"
#include "dice_detection.h"
#include "json_handling.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <string>
#include <csignal>
#include <atomic>

std::atomic<bool> running(true);

void signal_handler(int signum) {
    running = false;
}

static void process_message(const std::string& msg);
static void handle_registration();
static void handle_turn(int player);
static void handle_endgame();
static void handle_token_message(const std::string& msg);

static State gameState;

static const std::vector<std::string> TILE_ACTIONS = {
    "none",        // tile 0 (start)
    "points-2",    // tile 1
    "none",        // tile 2
    "minigame",    // tile 3
    "none",        // tile 4
    "move-2",      // tile 5
    "none",        // tile 6
    "minigame",    // tile 7
    "points+2",    // tile 8
    "none",        // tile 9
    "move+2",     // tile 10
    "none"         // tile 11 (end)
};

static void handle_token_message(const std::string& msg) {
    int token, value;
    // parse TOKEN[player]:[0 or 1]
    size_t colon = msg.find(':');
    token = std::stoi(msg.substr(5, colon - 5));
    value = std::stoi(msg.substr(colon + 1));
    // update JSON
    gameState.read("state.json");
    gameState.players[token - 1].out = !value;
    gameState.write("state.json");
}

static void applyTileAction(int player, const std::string& action) {
    if (action == "none") {
        return;
    }
    else if (action == "minigame") {
        //CHANGES SCREEN TO TELL PLAYERS TO GET TOKEN OFF BOARF
        std::this_thread::sleep_for(std::chrono::milliseconds(1000*2));
        gameState.state = 2;
        gameState.write("state.json");
        gameState.minigameWinner = 0;

        //continues to read until all players are off the board and in token slot
        while(gameState.players[0].out == 1 || gameState.players[1].out == 1 || gameState.players[2].out == 1 || gameState.players[3].out == 1) {
            gameState.read("state.json");
            std::this_thread::sleep_for(std::chrono::milliseconds(100*1));
            std::string input = uart_receive();
            if (input.find("TOKEN") == 0) {
                handle_token_message(input);
            }
        }


        //debug on who triggered the minigam
        std::this_thread::sleep_for(std::chrono::milliseconds(1000*10));
        std::cout << "Player " << player << " triggered a minigame!\n";
        // TODO: launch minigame
        //tells minigame who are the active players
        gameState.writeMini("config.json");
        //opens minigame
        uart_send("MINIGAME\n");
        std::this_thread::sleep_for(std::chrono::milliseconds(1000*1));
        system("./pong &");
        //wait until minigame is done / has a winner
        while (gameState.minigameWinner <= 0) {
            gameState.readMini("config.json");
            std::this_thread::sleep_for(std::chrono::milliseconds(1000*1));
        }
        while (uart_receive() != "MINI_ENDED") {
            uart_send("MINIGAME_STOP\n"); //Harini
            std::this_thread::sleep_for(std::chrono::milliseconds(70));
        }
        
        std::cout << "can tell it needs to end the game\n";

        //stupid long visual wait, return to board game path screen,  then close minigame
        std::this_thread::sleep_for(std::chrono::milliseconds(1000*5));
        gameState.state = 1;
        gameState.write("state.json");
        std::cout << "CHANGES THE STATE\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(1000*5));
        system("pkill -x pong");
        gameState.players[gameState.minigameWinner - 1].score += 2;
        

    }
    else if (action == "points+2") {
        gameState.players[player-1].score += 2;
    }
    else if (action == "points-2") {
        gameState.players[player-1].score -= 2;
    }
    else if (action == "move-2") {
        gameState.players[player-1].location -= 2;
        if (gameState.players[player-1].location < 0)
            gameState.players[player-1].location = 0;
    }
    else if (action == "move+2") {
        gameState.players[player-1].location += 2;
        if (gameState.players[player-1].location > 11)
            gameState.players[player-1].location = 11;
    }
}

void game_loop() {
    while (running) {
        std::string msg = uart_receive();
        if (!msg.empty()) {
            process_message(msg);
        }
    }
}


static void process_message(const std::string& msg) {
    std::cout << "Processing message: " << msg << "\n";
    if (msg == "REGISTER\n") {
        handle_registration();
    }
    else if (msg.find("PLAYER:") == 0) {
        int player = std::stoi(msg.substr(7));
        handle_turn(player);
    }
    else if (msg == "ENDGAME") {
        handle_endgame();
    }
    else {
        std::cout << "Unknown message: " << msg << "\n";
    }
}


static void handle_registration() {
    std::cout << "Enter Player Registration\n";
    // initialize variables to 0
    gameState.resetData();
    gameState.write("state.json");

    // call the graphics_release to start registration
    system("pkill -f graphics_release");
    // system("./graphics_release &");
    int result = system("cd /home/georgia/pixel-party && nohup ./graphics_release > godot.log 2>&1 &");
    std::cout << "Godot launch command returned: " << result << std::endl; 
 

    // every time a player removes token, write to json
    int activeCount = 0;
     
    while (running) {      
        while (running) {
            std::string input = uart_receive();
            if (input == "START") {
                break;
            }
            else if (input.find("TOKEN") == 0) {
                handle_token_message(input);
            }
        }

        // on start button, mark any out players as active
        gameState.read("state.json");
        gameState.updateActivePlayers();
        gameState.write("state.json");

        // count active players
        activeCount = 0;
        for (int i = 0; i < State::NUM_PLAYERS; i++) {
            if (gameState.players[i].active == 1) {
                activeCount++;
            }
        }
        // Check at least 2 active players
        if (activeCount < 2) {
            std::cout << "Need at least 2 active players. Restarting registration...\n";
            gameState.start = 1;
            gameState.write("state.json");
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            gameState.start = 0;
            gameState.write("state.json");
            continue;
        }

        break; // valid game start
    }

    // then write to json to toggle the start value 1 0
    gameState.read("state.json");
    gameState.totalActive = activeCount;
    gameState.start = 1;
    gameState.write("state.json");
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
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

    // move to next graphics_release state: 1 = board game
    gameState.state = 1;
    gameState.write("state.json");

    uart_send("NEXT:" + std::to_string(firstPlayer) + "\n");
}


static void handle_turn(int player) {
    std::cout << "Player Turn for player " << player << "\n";

    // Dice detection
    int dice_val = runDiceDetection();
    // int dice_val = 3;


    // Move player with dice 
    gameState.read("state.json");
    gameState.players[player-1].location += dice_val;
    if (gameState.players[player-1].location > 11)
            gameState.players[player-1].location = 11;
    gameState.write("state.json");
    std::this_thread::sleep_for(std::chrono::milliseconds(1000*dice_val));

    // Do tile action
    int pos = gameState.players[player-1].location;
    if (pos < 11) {
        gameState.write("state.json");
        std::this_thread::sleep_for(std::chrono::milliseconds(1000*8));
        applyTileAction(player, TILE_ACTIONS[pos]);
        gameState.write("state.json");
    }

    // Get next player or end of game
    int next = player;
    for (int i = 0; i < State::NUM_PLAYERS; i++) {
        next++;
        if (next > 4) next = 1;
        if (gameState.players[next - 1].active == 1 && !gameState.checkPlayerComplete(next)) {
            uart_send("NEXT:" + std::to_string(next) + "\n");
            return;
        }
    }

    // no players left, -1 is end game
    uart_send("NEXT:" + std::to_string(-1) + "\n");
}


static void handle_endgame() {
    std::cout << "Endgame\n";
    gameState.read("state.json");
    
    // get winner player and score
    // start with the first active player
    int win = 0;
    for (int i = 0; i < State::NUM_PLAYERS; i++) {
        if (gameState.players[i].active == 1) {
            win = i + 1; // convert index (0–3) to player number (1–4)
            break;
        }
    }
    // then see if any other active players beat that score
    int bestScore = gameState.players[win-1].score;

    for (int i = 0; i < State::NUM_PLAYERS; i++) {
        if (!gameState.players[i].active) continue;
        if (gameState.players[i].score > bestScore) {
            bestScore = gameState.players[i].score;
            win = i + 1;
        }
    }

    gameState.winner = win;
    gameState.winnerScore = bestScore;
    // Endgame graphics_release with state 3
    gameState.state = 3;
    gameState.write("state.json");
}


int main() {
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    if (!uart_init("/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_A50285BI-if00-port0", B9600)) {
        return 1;
    }
    process_message("REGISTER\n");
    game_loop();
    std::cout << "Shutting down cleanly...\n";
    uart_close();
    system("pkill -x graphics_release");
    return 0;
}
