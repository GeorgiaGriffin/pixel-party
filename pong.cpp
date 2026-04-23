// /*******************************************************************************************
// *   raylib - Pong
// *   Angel G. Cuartero. 2019-03-07.
// ********************************************************************************************/

// #include "raylib.h"
// #include <stdio.h>
// #include <stdlib.h>

// #define CALIBER 12

// // User-defined types.
// typedef enum GameScreen { TITLE = 0, GAMEPLAY, ENDING } GameScreen;
// typedef enum Direction { UP = 0, DOWN } Direction;

// // Global variables. They are global indeed.
// Rectangle screen, playableBorder, top, bottom, ball, leftRacket, rightRacket;
// int rightScore = 0, leftScore = 0, scoreWidth, winner;

// // Prototypes.
// void InitializeElements(void);
// void ServeBall(void);
// void MoveBall(void);
// void MoveRacket(Rectangle *pRacket, Direction pDir);

// // Initialize window and primary game elements.
// // --------------------------------------------
// void InitializeElements(void)
// {
//     InitWindow(0, 0, "Pong");
//     // Calculate size, position and inner limits of window.
//     screen = (Rectangle){0, 0, GetScreenWidth()/2, GetScreenHeight()/2};
//     playableBorder = (Rectangle){CALIBER, CALIBER, screen.width - (2*CALIBER) , screen.height - (2*CALIBER)};
//     top = (Rectangle) {screen.x, screen.y, playableBorder.width, playableBorder.y};
//     bottom = (Rectangle) {screen.x, playableBorder.height+CALIBER, screen.width, screen.y};
//     SetWindowPosition(screen.width/2, screen.height/2);
//     SetWindowSize(screen.width, screen.height);
//     SetTargetFPS(60);

//     // Initialize elements.
//     ball = (Rectangle) {5*CALIBER, playableBorder.height, CALIBER, CALIBER};
//     leftRacket = (Rectangle) {playableBorder.x + CALIBER, playableBorder.height/2, CALIBER, 5*CALIBER};
//     rightRacket = (Rectangle) {playableBorder.width - CALIBER, playableBorder.height/2, CALIBER, 5*CALIBER};
//     scoreWidth = MeasureText("00", 60);
// }

// // Manage ball movement.
// // ---------------------
// void MoveBall(void)
// {
//     static int xx = CALIBER/2;
//     static int yy = CALIBER/2;

//     // Check collision with rackets and ball has not surpassed rackets.
//     if ((CheckCollisionRecs(ball, leftRacket) && ball.x < leftRacket.x + leftRacket.width) ||
//         (CheckCollisionRecs(ball, rightRacket) && ball.x > rightRacket.x - rightRacket.width))
//         xx = -xx;
//     else
//         if (CheckCollisionRecs(ball, top) || CheckCollisionRecs(ball, bottom))
//             yy = -yy;
//         else
//         {
//             // Score.
//             if (ball.x < screen.x)
//             {
//                 ++rightScore;
//                 ServeBall();
//             }
//             else if (ball.x > screen.width)
//             {
//                 ++leftScore;
//                 ServeBall();
//             }
//         }

//     // Move ball.
//     ball.x += xx;
//     ball.y += yy;
// }

// // Manage racket movement.
// // -----------------------
// void MoveRacket(Rectangle *pRacket, Direction pDir)
// {
//     int step = (pDir == UP)? -CALIBER/2: CALIBER/2;

//     if ((CheckCollisionRecs(top, *pRacket) && pDir == UP) ||
//         (CheckCollisionRecs(bottom, *pRacket) && pDir == DOWN))
//             return;
//     pRacket->y += step;
// }

// // Serve ball after scoring.
// // -------------------------
// void ServeBall(void)
// {
//     ball.x = playableBorder.width/2;
//     ball.y = GetRandomValue(playableBorder.y + 10, playableBorder.height);
// }

// // Start game.
// // -----------
// int main(void)
// {
//     GameScreen currentScreen = TITLE;
//     InitializeElements();

//     // Main loop.
//     while (!WindowShouldClose()) // Check ESC key.
//     {
//         // Updating.
//         switch(currentScreen)
//         {
//             case TITLE:
//             {
//                 if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
//                     currentScreen = GAMEPLAY;
//             } break;
//             case GAMEPLAY:
//             {
//                 MoveBall();

//                 // Check racket keys.
//                 if (IsKeyDown(KEY_Q))
//                     MoveRacket(&leftRacket, UP);
//                 else if (IsKeyDown(KEY_A))
//                     MoveRacket(&leftRacket, DOWN);

//                 if (IsKeyDown(KEY_I))
//                     MoveRacket(&rightRacket, UP);
//                 else if (IsKeyDown(KEY_J))
//                     MoveRacket(&rightRacket, DOWN);

//                 if ((leftScore >= 11) || (rightScore >= 11))
//                 {
//                     if (abs(leftScore - rightScore) < 2)
//                         break;
//                     winner = (leftScore > rightScore)? 1 : 2;
//                     rightScore = leftScore = 0; // Reset Score.
//                     currentScreen = ENDING;
//                 }
//             } break;
//             case ENDING:
//             {
//                 if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
//                     currentScreen = GAMEPLAY;
//             } break;
//             default: break;
//         }

//         // Rendering.
//         BeginDrawing();
//         switch(currentScreen)
//         {
//             case TITLE:
//             {
//                 ClearBackground(BLACK);
//                 DrawText("PONG", 120, 20, 120, GRAY);
//                 DrawText("Based on Atari PONG", 120, 140, 60, GRAY);
//                 DrawText("Programmed with Raylib by Angel G. Cuartero", 120, 220, 20, GRAY);
//                 DrawText("Player 1: Q, A", 120, 390, 20, GRAY);
//                 DrawText("Player 2: I, J", 120, 420, 20, GRAY);
//                 DrawText("Press ENTER to PLAY", 120, 450, 20, GRAY);
//                 DrawText("Press ESCAPE to QUIT", 120, 480, 20, GRAY);

//             } break;
//             case GAMEPLAY:
//             {
//                 // Draw court.
//                 DrawRectangle(screen.x, screen.y, screen.width, screen.height, GRAY);
//                 DrawRectangle(screen.x, playableBorder.y, screen.width, playableBorder.height, BLACK);
//                 DrawRectangle((screen.width/2) - 5, playableBorder.y, CALIBER, playableBorder.height, GRAY);
//                 // Draw score.
//                 DrawText(TextFormat("%02d", leftScore), (screen.width/2) - 50 - scoreWidth, 50, 60, GRAY);
//                 DrawText(TextFormat("%02d", rightScore), (screen.width/2) + 50, 50, 60, GRAY);
//                 // Draw ball.
//                 DrawRectangle(ball.x, ball.y, ball.width, ball.height, WHITE);
//                 // Draw rackets.
//                 DrawRectangle(leftRacket.x, leftRacket.y, leftRacket.width, leftRacket.height, WHITE);
//                 DrawRectangle(rightRacket.x, rightRacket.y, rightRacket.width, rightRacket.height, WHITE);
//             } break;
//             case ENDING:
//             {
//                 ClearBackground(BLACK);
//                 DrawText(TextFormat("Winner is Player %d", winner), 120 , 50, 60, GRAY);
//                 DrawText("Press ENTER to PLAY AGAIN", 120, 420, 20, GRAY);
//                 DrawText("Press ESCAPE to QUIT", 120, 450, 20, GRAY);
//             } break;
//             default: break;
//             }
//         EndDrawing();
//     }

//     CloseWindow();
//     return 0;
// }



/*******************************************************************************************
*   Scalable Pong (2–4 players)
********************************************************************************************/

#include "raylib.h"
#include <stdlib.h>
#include <math.h>
#include "game_config.hpp"

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

Rectangle screen, playableBorder, ball, top, bottom;
Rectangle left, right;
Paddle paddles[MAX_PLAYERS];
int scores[MAX_PLAYERS];
// int playerCount = 4; // change: 2, 3, or 4
int playerOne;
int playerTwo;
int playerThree;
int playerFour;
// int playerCount = playerOne + playerTwo + playerThree + playerFour; // change: 2, 3, or 4
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
    InitWindow(1280, 1024, "Scalable Pong");  // temp size
    ToggleFullscreen();                      // go fullscreen

    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
        // ToggleFullscreen();
    
    SetTargetFPS(60);

    screen = (Rectangle){0, 0, screenWidth, screenHeight};
    playableBorder = (Rectangle){CALIBER, CALIBER, screenWidth - 2*CALIBER, screenHeight - 2*CALIBER};
    top = (Rectangle) {screen.x, screen.y, playableBorder.width, playableBorder.y};
    bottom = (Rectangle) {screen.x, playableBorder.height+CALIBER, screen.width, screen.y};
    left = (Rectangle){0, 0, CALIBER, screen.height};
    right = (Rectangle){screen.width - CALIBER, 0, CALIBER, screen.height};
    
    // Ball
    ball = (Rectangle){
        screenWidth / 2.0f - CALIBER / 2.0f,
        screenHeight / 2.0f - CALIBER / 2.0f,
        CALIBER,
        CALIBER
    };
    ballVelX = CALIBER/2;
    ballVelY = CALIBER/2;

    ResetScores();
    if (playerTwo == 1) {
        // LEFT paddle
        paddles[1].rect = (Rectangle){CALIBER, screenHeight / 2.0f - (5*CALIBER)/2, CALIBER, 5*CALIBER};
        paddles[1].type = VERTICAL;
    }

    if (playerFour == 1) {
        // RIGHT paddle
        paddles[3].rect = (Rectangle){screenWidth - 2*CALIBER, screenHeight / 2.0f - (5*CALIBER)/2, CALIBER, 5*CALIBER};
        paddles[3].type = VERTICAL;
    }

    if (playerThree == 1) {
        // BOTTOM paddle
        paddles[2].rect = (Rectangle){screenWidth / 2.0f - (5*CALIBER)/2, screenHeight - 2*CALIBER, 5*CALIBER, CALIBER};
        paddles[2].type = HORIZONTAL;
    }

    if (playerOne == 1) {
        // TOP paddle
        paddles[0].rect = (Rectangle){screenWidth / 2.0f - (5*CALIBER)/2, CALIBER, 5*CALIBER, CALIBER};
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
    

    // if (playerCount == 4) {
    //     // Wall scoring
    //     if (ball.x < 0)
    //     {
    //         scores[1]++; // left missed
    //         scores[2]++;
    //         scores[3]++;
    //         ServeBall();
    //     }
    //     else if (ball.x > screen.width)
    //     {
    //         scores[0]++; // right missed
    //         scores[2]++;
    //         scores[3]++;
    //         ServeBall();
    //     }
    //     if (ball.y > screen.height)
    //     {
    //         scores[0]++; // bottom missed
    //         scores[1]++;
    //         scores[3]++;
    //         ServeBall();
    //     }
    //     if (ball.y < 0)
    //     {
    //         scores[0]++; // top missed
    //         scores[1]++;
    //         scores[2]++;
    //         ServeBall();
    //     }
    // }
    // else if (playerCount == 3) {
    //     // Wall scoring
    //     if (CheckCollisionRecs(ball, top)) ballVelY = -ballVelY;
    //     else {
    //         if (ball.x < 0)
    //         {
    //             scores[1]++; // left missed
    //             scores[2]++;
    //             ServeBall();
    //         }
    //         else if (ball.x > screen.width)
    //         {
    //             scores[0]++; // right missed
    //             scores[2]++;
    //             ServeBall();
    //         }
    //         if (ball.y > screen.height)
    //         {
    //             scores[0]++; // bottom missed
    //             scores[1]++;
    //             ServeBall();
    //         }
    //     }
        
    // }
    // else {
    //     // Wall scoring
    //     if (CheckCollisionRecs(ball, top) || (CheckCollisionRecs(ball, bottom))) ballVelY = -ballVelY;
    //     else {
    //         if (ball.x < 0)
    //         {
    //             scores[1]++; // left missed

    //             ServeBall();
    //         }
    //         else if (ball.x > screen.width)
    //         {
    //             scores[0]++; // right missed

    //             ServeBall();
    //         }
    //     }
    // }

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
void MovePaddles(void)
{
    int step = CALIBER/2;
    // IsKeyDown(KEY_Q)
    // if (IsKeyDown(KEY_Q)) config.playerTwoMove = -1;
    // else if (IsKeyDown(KEY_A)) config.playerTwoMove = 1;
    // else config.playerTwoMove = 0;    

    // if (IsKeyDown(KEY_I)) config.playerFourMove = 1;
    // else if (IsKeyDown(KEY_J)) config.playerFourMove = -1;
    // else config.playerFourMove = 0;    

    // if (IsKeyDown(KEY_Z)) config.playerOneMove = -1;
    // else if (IsKeyDown(KEY_X)) config.playerOneMove = 1;
    // else config.playerOneMove = 0;    

    // if (IsKeyDown(KEY_N)) config.playerThreeMove = -1;
    // else if (IsKeyDown(KEY_M)) config.playerThreeMove = 1;
    // else config.playerThreeMove = 0; 

    // if (IsKeyPressed(KEY_ONE)) config.playerOnePowerUp = 1;
    // else config.playerOnePowerUp = 0;
    // if (IsKeyPressed(KEY_TWO)) config.playerTwoPowerUp = 1;
    // else config.playerTwoPowerUp = 0;
    // if (IsKeyPressed(KEY_THREE)) config.playerThreePowerUp = 1;
    // else config.playerThreePowerUp = 0;
    // if (IsKeyPressed(KEY_FOUR)) config.playerFourPowerUp = 1;
    // else config.playerFourPowerUp = 0;
    
    // JOYSTICK INTEGRATION CODE START

    const float DEADZONE = 0.2f;
    config.read("config.json");

    // Player 1 --> Gamepad 0
    if (IsGamepadAvailable(0)) {
        float axis = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);

        if (axis < -DEADZONE) config.playerOneMove = -1;
        else if (axis > DEADZONE) config.playerOneMove = 1;
        else config.playerOneMove = 0;

        config.playerOnePowerUp =
            IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
    } else {
        if (IsKeyDown(KEY_Z)) config.playerOneMove = -1;
        else if (IsKeyDown(KEY_X)) config.playerOneMove = 1;
        else config.playerOneMove = 0;  
        if (IsKeyPressed(KEY_ONE)) config.playerOnePowerUp = 1;
        else config.playerOnePowerUp = 0; 
    }

    // Player 2 --> Gamepad 1
    if (IsGamepadAvailable(1)) {
        float axis = GetGamepadAxisMovement(1, GAMEPAD_AXIS_LEFT_Y);

        if (axis < -DEADZONE) config.playerTwoMove = -1;
        else if (axis > DEADZONE) config.playerTwoMove = 1;
        else config.playerTwoMove = 0;

        config.playerTwoPowerUp =
            IsGamepadButtonPressed(1, GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
    } else {
        if (IsKeyDown(KEY_Q)) config.playerTwoMove = -1;
        else if (IsKeyDown(KEY_A)) config.playerTwoMove = 1;
        else config.playerTwoMove = 0;     
        if (IsKeyPressed(KEY_TWO)) config.playerTwoPowerUp = 1;
        else config.playerTwoPowerUp = 0; 
    }

    // Player 3 --> Gamepad 2
    if (IsGamepadAvailable(2)) {
        float axis = GetGamepadAxisMovement(2, GAMEPAD_AXIS_LEFT_X);

        if (axis < -DEADZONE) config.playerThreeMove = -1;
        else if (axis > DEADZONE) config.playerThreeMove = 1;
        else config.playerThreeMove = 0;

        config.playerThreePowerUp =
            IsGamepadButtonPressed(2, GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
    } else {
        if (IsKeyDown(KEY_N)) config.playerThreeMove = -1;
        else if (IsKeyDown(KEY_M)) config.playerThreeMove = 1;
        else config.playerThreeMove = 0; 
        if (IsKeyPressed(KEY_THREE)) config.playerThreePowerUp = 1;
        else config.playerThreePowerUp = 0;    
    }

    // Player 4 --> Gamepad 3
    if (IsGamepadAvailable(3)) {
        float axis = GetGamepadAxisMovement(3, GAMEPAD_AXIS_LEFT_Y);

        if (axis < -DEADZONE) config.playerFourMove = -1;
        else if (axis > DEADZONE) config.playerFourMove = 1;
        else config.playerFourMove = 0;

        config.playerFourPowerUp =
            IsGamepadButtonPressed(3, GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
    } else {
        if (IsKeyDown(KEY_I)) config.playerFourMove = 1;
        else if (IsKeyDown(KEY_J)) config.playerFourMove = -1;
        else config.playerFourMove = 0;  
        if (IsKeyPressed(KEY_FOUR)) config.playerFourPowerUp = 1;
        else config.playerFourPowerUp = 0;
    }

    // JOYSTICK INTEGRATION CODE END



    config.write("config.json");
    
    config.read("config.json");


    if (playerTwo == 1)
    {
        // LEFT (Q/A)
        if (config.playerTwoMove != 0) paddles[1].rect.y += config.playerTwoMove * step * (powerActive[1] ? POWERUP_SPEED_MULT : 1);
        if (paddles[1].rect.y <= 0) paddles[1].rect.y = 0;
        if (paddles[1].rect.y >= playableBorder.height - (paddles[1].rect.height / 2)) paddles[1].rect.y = playableBorder.height - (paddles[1].rect.height / 2);
    }

    if (playerFour == 1) {
        // RIGHT (I/J)
        if (config.playerFourMove != 0) paddles[3].rect.y += config.playerFourMove * step * (powerActive[3] ? POWERUP_SPEED_MULT : 1);
        if (paddles[3].rect.y <= 0) paddles[3].rect.y = 0;
        if (paddles[3].rect.y >= playableBorder.height - (paddles[3].rect.height / 2)) paddles[3].rect.y = playableBorder.height - (paddles[3].rect.height / 2);
    }


    if (playerThree == 1)
    {
        // BOTTOM (N/M)
        if (config.playerThreeMove != 0) paddles[2].rect.x += config.playerThreeMove * step * (powerActive[2] ? POWERUP_SPEED_MULT : 1);
        if (paddles[2].rect.x <= 0) paddles[2].rect.x = 0;
        if (paddles[2].rect.x >= playableBorder.width - (paddles[2].rect.width / 2)) paddles[2].rect.x = playableBorder.width - (paddles[2].rect.width / 2);

    }

    if (playerOne == 1)
    {
        // TOP (Z/X)
        if (config.playerOneMove != 0) paddles[0].rect.x += config.playerOneMove * step * (powerActive[0] ? POWERUP_SPEED_MULT : 1);
        if (paddles[0].rect.x <= 0) paddles[0].rect.x = 0;
        if (paddles[0].rect.x >= playableBorder.width - (paddles[0].rect.width / 2)) paddles[0].rect.x = playableBorder.width - (paddles[0].rect.width / 2);

    }
    // Activate powerups
    if (config.playerOnePowerUp && playerOne && powerUses[0] > 0 && !powerActive[0]) {
        powerActive[0] = true;
        powerTimer[0] = POWERUP_DURATION;
        powerUses[0]--;
    }
    if (config.playerTwoPowerUp && playerTwo && powerUses[1] > 0 && !powerActive[1]) {
        powerActive[1] = true;
        powerTimer[1] = POWERUP_DURATION;
        powerUses[1]--;
    }
    if (config.playerThreePowerUp && playerThree && powerUses[2] > 0 && !powerActive[2]) {
        powerActive[2] = true;
        powerTimer[2] = POWERUP_DURATION;
        powerUses[2]--;
    }
    if (config.playerFourPowerUp && playerFour && powerUses[3] > 0 && !powerActive[3]) {
        powerActive[3] = true;
        powerTimer[3] = POWERUP_DURATION;
        powerUses[3]--;
    }
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (powerActive[i]) {
            powerTimer[i] -= GetFrameTime();
            if (powerTimer[i] <= 0) {
                powerActive[i] = false;
            }
        }
    }
}

// --------------------------------------------
// void ServeBall(void)
// {
//     ball.x = screen.width / 2;
//     ball.y = screen.height / 2;

//     ballVelX = (GetRandomValue(0,1) ? 1 : -1) * CALIBER/2;
//     ballVelY = (GetRandomValue(0,1) ? 1 : -1) * CALIBER/2;
// }
void ServeBall(void)
{
    ball.x = screen.width / 2 - CALIBER / 2;
    ball.y = screen.height / 2 - CALIBER / 2;

    int speed = CALIBER / 2;

    // pick random direction components
    int vx = GetRandomValue(-speed, speed);
    int vy = GetRandomValue(-speed, speed);

    // enforce minimum magnitude so it's never flat
    int minComponent = 2; // tweak this (higher = more diagonal)

    if (abs(vx) < minComponent)
        vx = (vx < 0 ? -minComponent : minComponent);

    if (abs(vy) < minComponent)
        vy = (vy < 0 ? -minComponent : minComponent);

    // normalize to keep consistent speed (optional but nicer)
    float length = sqrtf(vx*vx + vy*vy);
    ballVelX = (int)(vx / length * speed);
    ballVelY = (int)(vy / length * speed);
}

// --------------------------------------------
int main(void)
{
    GameScreen currentScreen = GAMEPLAY;
    
    config.read("config.json");

    playerOne = config.playerOne;
    playerTwo = config.playerTwo;
    playerThree = config.playerThree;
    playerFour = config.playerFour;

    InitializeElements();

    while (!WindowShouldClose())
    {
        // Update
        switch (currentScreen)
        {
            case TITLE:
                if (IsKeyPressed(KEY_ENTER))
                    currentScreen = GAMEPLAY;
                break;

            case GAMEPLAY:
                MoveBall();
                MovePaddles();

                // winner = 0;

                if ((scores[0] >= 11) || (scores[1] >= 11) || (scores[2] >= 11) || (scores[3] >= 11)){
                    winner = -1;

                    for (int i = 0; i < 4; i++)
                    {
                        if ((i == 0 && !playerOne) ||
                            (i == 1 && !playerTwo) ||
                            (i == 2 && !playerThree) ||
                            (i == 3 && !playerFour))
                            continue;

                        if (winner == -1 || scores[i] > scores[winner])
                            winner = i;
                    }

                    for (int i = 0; i < 4; i++)
                    {
                        if (winner != i && scores[i] == scores[winner])
                            winner = -1;
                    }
                    
                    if (winner == -1) break;
                    currentScreen = ENDING;
                }


                break;

            case ENDING:
                config.winner = winner + 1;
                config.write("config.json");

                for (int i = 0; i < 4; i++)
                {
                    scores[i] = 0;
                    powerUses[i] = 3;
                    

                }
                if (IsKeyPressed(KEY_ENTER))
                    
                    currentScreen = GAMEPLAY;
                break;
        }

        // Draw
        BeginDrawing();
        ClearBackground(BLACK);

        if (currentScreen == TITLE)
        {
            const char* title = "SCALABLE PONG";
            int textWidth = MeasureText(title, 40);

            DrawText(title, screen.width/2 - textWidth/2, screen.height/4, 40, GRAY);
            DrawText("Press ENTER to Start", screen.width/2 - textWidth/2, screen.height/4, 20, GRAY);
        }
        else if (currentScreen == GAMEPLAY)
        {
            // Draw ball
            DrawRectangleRec(ball, WHITE);

            // Draw paddles
            // for (int i = 0; i < playerCount; i++)
            //     DrawRectangleRec(paddles[i].rect, WHITE);

            if (playerOne) DrawRectangleRec(paddles[0].rect, powerActive[0] ? RED : WHITE);
            if (playerTwo) DrawRectangleRec(paddles[1].rect, powerActive[1] ? RED : WHITE);
            if (playerThree) DrawRectangleRec(paddles[2].rect, powerActive[2] ? RED : WHITE);
            if (playerFour) DrawRectangleRec(paddles[3].rect, powerActive[3] ? RED : WHITE);

            // Draw scores
            if (playerOne) DrawText(TextFormat("P1: %d", scores[0]), 20, 20, 20, GRAY);

            if (playerTwo) DrawText(TextFormat("P2: %d", scores[1]), 20, 50, 20, GRAY);

            if (playerThree) DrawText(TextFormat("P3: %d", scores[2]), 20, 80, 20, GRAY);

            if (playerFour) DrawText(TextFormat("P4: %d", scores[3]), 20, 110, 20, GRAY);

            //display powerups left:
            if (playerOne) DrawText(TextFormat("P1 Boosts: %d", powerUses[0]), 150, 20, 20, GRAY);

            if (playerTwo) DrawText(TextFormat("P2 Boosts: %d", powerUses[1]), 150, 50, 20, GRAY);

            if (playerThree) DrawText(TextFormat("P3 Boosts: %d", powerUses[2]), 150, 80, 20, GRAY);

            if (playerFour) DrawText(TextFormat("P4 Boosts: %d", powerUses[3]), 150, 110, 20, GRAY);
        }
        else {
            ClearBackground(BLACK);
            DrawText(TextFormat("Winner is Player %d", winner + 1), screen.width/2 - MeasureText("Winner is Player 2", 60)/2 , screen.height/3, 60, GRAY);
            
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}