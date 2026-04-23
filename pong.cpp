// /*******************************************************************************************
// *   raylib - Pong
// *   Angel G. Cuartero. 2019-03-07.
// ********************************************************************************************/

/*******************************************************************************************
*   Scalable Pong (2–4 players)
********************************************************************************************/

#include "raylib.h"
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include "game_config.hpp"
#include "uart.h" 

#define CALIBER 12
#define MAX_PLAYERS 4

typedef enum GameScreen { TITLE = 0, GAMEPLAY, ENDING } GameScreen;
typedef enum PaddleType { VERTICAL = 0, HORIZONTAL } PaddleType;

// Paddle struct
typedef struct {
    Rectangle rect;
    PaddleType type;
} Paddle;


// config.read("config.json")

// Globals
#define POWERUP_DURATION 2.0f
#define POWERUP_SPEED_MULT 2

int powerUses[MAX_PLAYERS] = {3,3,3,3};
float powerTimer[MAX_PLAYERS] = {0};
bool powerActive[MAX_PLAYERS] = {false};

Rectangle screen, playableBorder, ball, top, bottom, left, right;
Paddle paddles[MAX_PLAYERS];
int scores[MAX_PLAYERS];
int playerOne, playerTwo, playerThree, playerFour;
int winner = 0;
int ballVelX, ballVelY;
GameConfig config;

// Prototypes
void InitializeElements(void);
void MoveBall(void);
void MovePaddles(void);
void ServeBall(void);
void ResetScores(void);

// --------------------------------------------
void InitializeElements(void)
{
    InitWindow(800, 600, "Scalable Pong");
    SetTargetFPS(60);

    screen = (Rectangle){0, 0, 800, 600};
    playableBorder = (Rectangle){CALIBER, CALIBER, 800 - 2*CALIBER, 600 - 2*CALIBER};
    top = (Rectangle) {screen.x, screen.y, playableBorder.width, playableBorder.y};
    bottom = (Rectangle) {screen.x, playableBorder.height+CALIBER, screen.width, screen.y};
    left = (Rectangle){0, 0, CALIBER, screen.height};
    right = (Rectangle){screen.width - CALIBER, 0, CALIBER, screen.height};
    
    // Ball
    ball = (Rectangle){400, 300, CALIBER, CALIBER};
    ballVelX = CALIBER/2;
    ballVelY = CALIBER/2;

    ResetScores();
    if (playerTwo == 1) {
        // LEFT paddle
        paddles[1].rect = (Rectangle){CALIBER, 250, CALIBER, 5*CALIBER};
        paddles[1].type = VERTICAL;
    }

    if (playerFour == 1) {
        // RIGHT paddle
        paddles[3].rect = (Rectangle){800 - 2*CALIBER, 250, CALIBER, 5*CALIBER};
        paddles[3].type = VERTICAL;
    }

    if (playerThree == 1) {
        // BOTTOM paddle
        paddles[2].rect = (Rectangle){350, 600 - 2*CALIBER, 5*CALIBER, CALIBER};
        paddles[2].type = HORIZONTAL;
    }

    if (playerOne == 1) {
        // TOP paddle
        paddles[0].rect = (Rectangle){350, CALIBER, 5*CALIBER, CALIBER};
        paddles[0].type = HORIZONTAL;
    }
}

// --------------------------------------------
void ResetScores(void)
{
    for (int i = 0; i < MAX_PLAYERS; i++)
        scores[i] = 0;
}

// --------------------------------------------
void MoveBall(void)
{
    // Paddle collisions
    
    if (playerOne && CheckCollisionRecs(ball, paddles[0].rect))
        ballVelY = -ballVelY;

    if (playerTwo && CheckCollisionRecs(ball, paddles[1].rect))
        ballVelX = -ballVelX;

    if (playerThree && CheckCollisionRecs(ball, paddles[2].rect))
        ballVelY = -ballVelY;

    if (playerFour && CheckCollisionRecs(ball, paddles[3].rect))
        ballVelX = -ballVelX;
    

    if (playerOne == 1) {
        
        if (ball.y < 0) {
            if (playerFour) scores[3]++; // top missed
            if (playerTwo) scores[1]++;
            if (playerThree) scores[2]++;
            ServeBall();
        }
    }
    else if (CheckCollisionRecs(ball, top)) ballVelY = -ballVelY;
    
    
    if (playerTwo == 1) {
        if (ball.x < 0) {
            if (playerThree) scores[2]++; // left missed
            if (playerOne) scores[0]++;
            if (playerFour) scores[3]++;
            ServeBall();
        }
    }
    else if (CheckCollisionRecs(ball, left)) ballVelX = -ballVelX;
    
    if (playerThree == 1) {
       if (ball.y > screen.height)
        {
            if (playerOne) scores[0]++; // bottom missed
            if (playerTwo) scores[1]++;
            if (playerFour) scores[3]++;
            ServeBall();
        }
    }
    else if (CheckCollisionRecs(ball, bottom)) ballVelY = -ballVelY;

    if (playerFour == 1) {
        if (ball.x > screen.width)
        {
            if (playerOne) scores[0]++; // right missed
            if (playerThree) scores[2]++;
            if (playerTwo) scores[1]++;
            ServeBall();
        }
    }
    else if (CheckCollisionRecs(ball, right)) ballVelX = -ballVelX;

    ball.x += ballVelX;
    ball.y += ballVelY;
}

// --------------------------------------------
void MovePaddles(void) {
    int step = CALIBER / 2;

    // Drain the buffer to get the MOST RECENT packet
    std::string latestLine = "";
    std::string currentLine = "";
    
    // Grab every completed line currently waiting in the serial buffer
    while (true) {
        currentLine = uart_receive();
        if (currentLine.empty()) break; 
        latestLine = currentLine;
    }

    // Only parse the very last complete line we received
    if (!latestLine.empty()) {
        ParseUartInput(latestLine);
    }

    // 2. Process Player Movement
    // Top Paddle (Player 1) - Horizontal
    if (playerOne && config.playerOneMove != 0) {
        paddles[0].rect.x += config.playerOneMove * step * (powerActive[0] ? POWERUP_SPEED_MULT : 1);
        if (paddles[0].rect.x <= CALIBER) paddles[0].rect.x = CALIBER;
        if (paddles[0].rect.x >= playableBorder.width - paddles[0].rect.width + CALIBER) 
            paddles[0].rect.x = playableBorder.width - paddles[0].rect.width + CALIBER;
    }

    // Left Paddle (Player 2) - Vertical
    if (playerTwo && config.playerTwoMove != 0) {
        paddles[1].rect.y += config.playerTwoMove * step * (powerActive[1] ? POWERUP_SPEED_MULT : 1);
        if (paddles[1].rect.y <= CALIBER) paddles[1].rect.y = CALIBER;
        if (paddles[1].rect.y >= playableBorder.height - paddles[1].rect.height + CALIBER) 
            paddles[1].rect.y = playableBorder.height - paddles[1].rect.height + CALIBER;
    }

    // Bottom Paddle (Player 3) - Horizontal
    if (playerThree && config.playerThreeMove != 0) {
        paddles[2].rect.x += config.playerThreeMove * step * (powerActive[2] ? POWERUP_SPEED_MULT : 1);
        if (paddles[2].rect.x <= CALIBER) paddles[2].rect.x = CALIBER;
        if (paddles[2].rect.x >= playableBorder.width - paddles[2].rect.width + CALIBER) 
            paddles[2].rect.x = playableBorder.width - paddles[2].rect.width + CALIBER;
    }

    // Right Paddle (Player 4) - Vertical
    if (playerFour && config.playerFourMove != 0) {
        paddles[3].rect.y += config.playerFourMove * step * (powerActive[3] ? POWERUP_SPEED_MULT : 1);
        if (paddles[3].rect.y <= CALIBER) paddles[3].rect.y = CALIBER;
        if (paddles[3].rect.y >= playableBorder.height - paddles[3].rect.height + CALIBER) 
            paddles[3].rect.y = playableBorder.height - paddles[3].rect.height + CALIBER;
    }

    // 3. Process Power-ups for all players
    int buttons[4] = {config.playerOnePowerUp, config.playerTwoPowerUp, config.playerThreePowerUp, config.playerFourPowerUp};
    bool activeStates[4] = {playerOne != 0, playerTwo != 0, playerThree != 0, playerFour != 0};

    for (int i = 0; i < MAX_PLAYERS; i++) {
        // Trigger if button is pressed (1), player is in game, has uses, and not already active
        if (buttons[i] == 1 && activeStates[i] && powerUses[i] > 0 && !powerActive[i]) {
            powerActive[i] = true;
            powerTimer[i] = POWERUP_DURATION;
            powerUses[i]--;
        }

        // Handle active timer countdown
        if (powerActive[i]) {
            powerTimer[i] -= GetFrameTime();
            if (powerTimer[i] <= 0) powerActive[i] = false;
        }
    }
}

void ServeBall(void)
{
    ball.x = screen.width / 2;
    ball.y = screen.height / 2;

    float speed = CALIBER / 2.0f;

    // random angle but avoid too vertical / too horizontal
    float angle = GetRandomValue(-60, 60) * DEG2RAD;

    // randomly flip left/right direction
    if (GetRandomValue(0, 1)) angle += PI;

    ballVelX = (int)(cosf(angle) * speed);
    ballVelY = (int)(sinf(angle) * speed);

    // safety: avoid 0 velocity (boring straight line)
    if (ballVelX == 0) ballVelX = (GetRandomValue(0,1) ? 1 : -1);
}

// --------------------------------------------
int main(void)
{
    // 1. Initial State Setup
    GameScreen currentScreen = GAMEPLAY;
    
    // Read the initial player assignments from config
    config.read("config.json");

    // Map configuration to local variables
    playerOne = config.playerOne;
    playerTwo = config.playerTwo;
    playerThree = config.playerThree;
    playerFour = config.playerFour;

    // 2. UART Initialization (Matching your example exactly)
    // Using /dev/ttyUSB0 and 9600 baud as requested
    if (!uart_init("/dev/ttyUSB0", 9600)) {
        // Using standard I/O since this is a basic setup
        std::cerr << "Failed to open UART on /dev/ttyUSB0" << std::endl;
        return 1;
    }

    // 3. Raylib Window and Element Initialization
    InitializeElements(); // Sets up screen, paddles, and ball

    // 4. Main Game Loop
    while (!WindowShouldClose())
    {
        // --- UPDATE LOGIC ---
        switch (currentScreen)
        {
            case TITLE:
                if (IsKeyPressed(KEY_ENTER))
                    currentScreen = GAMEPLAY;
                break;

            case GAMEPLAY:
                MoveBall();    // Handles physics and collisions
                MovePaddles(); // This now calls uart_receive() and ParseUartInput()

                // Check for a winner (first to 11 points)
                if ((scores[0] >= 11) || (scores[1] >= 11) || (scores[2] >= 11) || (scores[3] >= 11)) {
                    winner = -1;
                    for (int i = 0; i < MAX_PLAYERS; i++) {
                        // Check if the player is active
                        bool isActive = (i == 0 && playerOne) || (i == 1 && playerTwo) || 
                                        (i == 2 && playerThree) || (i == 3 && playerFour);
                        
                        if (!isActive) continue;

                        if (winner == -1 || scores[i] > scores[winner]) {
                            winner = i;
                        }
                    }

                    // Handle ties
                    for (int i = 0; i < MAX_PLAYERS; i++) {
                        if (winner != i && scores[i] == scores[winner]) {
                            winner = -1;
                        }
                    }
                    
                    if (winner != -1) currentScreen = ENDING;
                }
                break;

            case ENDING:
                if (IsKeyPressed(KEY_ENTER)) {
                    ResetScores();
                    for (int i = 0; i < MAX_PLAYERS; i++) {
                        powerUses[i] = 3;
                        powerActive[i] = false;
                    }
                    currentScreen = GAMEPLAY;
                }
                break;
        }

        // --- DRAW LOGIC ---
        BeginDrawing();
        ClearBackground(BLACK);

        if (currentScreen == TITLE) {
            DrawText("PIXEL PARTY PONG", 200, 100, 40, GRAY);
            DrawText("Press ENTER to Start", 250, 300, 20, GRAY);
        }
        else if (currentScreen == GAMEPLAY) {
            // Draw Ball
            DrawRectangleRec(ball, WHITE);

            // Draw Paddles (Highlight Red if power-up active)
            if (playerOne)   DrawRectangleRec(paddles[0].rect, powerActive[0] ? RED : WHITE);
            if (playerTwo)   DrawRectangleRec(paddles[1].rect, powerActive[1] ? RED : WHITE);
            if (playerThree) DrawRectangleRec(paddles[2].rect, powerActive[2] ? RED : WHITE);
            if (playerFour)  DrawRectangleRec(paddles[3].rect, powerActive[3] ? RED : WHITE);

            // Simple HUD for Player 1 (for basic testing)
            if (playerOne) {
                DrawText(TextFormat("P1 Score: %d", scores[0]), 20, 20, 20, GRAY);
                DrawText(TextFormat("P1 Boosts: %d", powerUses[0]), 20, 50, 20, GRAY);
            }
        }
        else if (currentScreen == ENDING) {
            DrawText(TextFormat("Winner: Player %d", winner + 1), 120, 50, 60, GRAY);
            DrawText("Press ENTER to restart", 120, 420, 20, GRAY);
        }

        EndDrawing();
    }

    // 5. Cleanup
    uart_close(); // Correctly closes the USB file descriptor
    CloseWindow();
    return 0;
}