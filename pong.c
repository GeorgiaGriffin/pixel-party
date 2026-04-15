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

#define CALIBER 12
#define MAX_PLAYERS 4

typedef enum GameScreen { TITLE = 0, GAMEPLAY, ENDING } GameScreen;
typedef enum PaddleType { VERTICAL = 0, HORIZONTAL } PaddleType;

// Paddle struct
typedef struct {
    Rectangle rect;
    PaddleType type;
} Paddle;

// Globals
Rectangle screen, playableBorder, ball, top, bottom;
Rectangle left, right;
Paddle paddles[MAX_PLAYERS];
int scores[MAX_PLAYERS];
// int playerCount = 4; // change: 2, 3, or 4
int playerOne = 1;
int playerTwo = 1;
int playerThree = 1;
int playerFour = 1;
// int playerCount = playerOne + playerTwo + playerThree + playerFour; // change: 2, 3, or 4
int winner = 0;

int ballVelX, ballVelY;

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
    int step = CALIBER;

    if (playerTwo == 1)
    {
        // LEFT (Q/A)
        if (IsKeyDown(KEY_Q)) paddles[1].rect.y -= step;
        if (IsKeyDown(KEY_A)) paddles[1].rect.y += step;
        if (paddles[1].rect.y <= 0) paddles[1].rect.y = 0;
        if (paddles[1].rect.y >= playableBorder.height - (paddles[1].rect.height / 2)) paddles[1].rect.y = playableBorder.height - (paddles[1].rect.height / 2);
    }

    if (playerFour == 1) {
        // RIGHT (I/J)
        if (IsKeyDown(KEY_I)) paddles[3].rect.y -= step;
        if (IsKeyDown(KEY_J)) paddles[3].rect.y += step;
        if (paddles[3].rect.y <= 0) paddles[3].rect.y = 0;
        if (paddles[3].rect.y >= playableBorder.height - (paddles[3].rect.height / 2)) paddles[3].rect.y = playableBorder.height - (paddles[3].rect.height / 2);
    }


    if (playerThree == 1)
    {
        // BOTTOM (N/M)
        if (IsKeyDown(KEY_N)) paddles[2].rect.x -= step;
        if (IsKeyDown(KEY_M)) paddles[2].rect.x += step;
        if (paddles[2].rect.x <= 0) paddles[2].rect.x = 0;
        if (paddles[2].rect.x >= playableBorder.width - (paddles[2].rect.width / 2)) paddles[2].rect.x = playableBorder.width - (paddles[2].rect.width / 2);

    }

    if (playerOne == 1)
    {
        // TOP (Z/X)
        if (IsKeyDown(KEY_Z)) paddles[0].rect.x -= step;
        if (IsKeyDown(KEY_X)) paddles[0].rect.x += step;
        if (paddles[0].rect.x <= 0) paddles[0].rect.x = 0;
        if (paddles[0].rect.x >= playableBorder.width - (paddles[0].rect.width / 2)) paddles[0].rect.x = playableBorder.width - (paddles[0].rect.width / 2);

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
    GameScreen currentScreen = GAMEPLAY;
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
                for (int i = 0; i < 4; i++)
                {
                    scores[i] = 0;
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
            DrawText("SCALABLE PONG", 200, 100, 40, GRAY);
            DrawText("Press ENTER to Start", 250, 300, 20, GRAY);
        }
        else if (currentScreen == GAMEPLAY)
        {
            // Draw ball
            DrawRectangleRec(ball, WHITE);

            // Draw paddles
            // for (int i = 0; i < playerCount; i++)
            //     DrawRectangleRec(paddles[i].rect, WHITE);

            if (playerOne) DrawRectangleRec(paddles[0].rect, WHITE);
            if (playerTwo) DrawRectangleRec(paddles[1].rect, WHITE);
            if (playerThree) DrawRectangleRec(paddles[2].rect, WHITE);
            if (playerFour) DrawRectangleRec(paddles[3].rect, WHITE);

            // Draw scores
            if (playerOne) DrawText(TextFormat("P1: %d", scores[0]), 20, 20, 20, GRAY);

            if (playerTwo) DrawText(TextFormat("P2: %d", scores[1]), 20, 50, 20, GRAY);

            if (playerThree) DrawText(TextFormat("P3: %d", scores[2]), 20, 80, 20, GRAY);

            if (playerFour) DrawText(TextFormat("P4: %d", scores[3]), 20, 110, 20, GRAY);
        }
        else {
             ClearBackground(BLACK);
            DrawText(TextFormat("Winner is Player %d", winner + 1), 120 , 50, 60, GRAY);
            DrawText("Press ENTER to PLAY AGAIN", 120, 420, 20, GRAY);
            DrawText("Press ESCAPE to QUIT", 120, 450, 20, GRAY);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}