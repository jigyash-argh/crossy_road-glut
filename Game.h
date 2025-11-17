#pragma once

#include <deque> 
#include <string>
#include "Lane.h"

// Define the states our game can be in
enum GameState {
    STATE_MENU,      
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_GAME_OVER
};

// Game class manages all game state and logic
class Game {
public:
    Game(); // Constructor
    void init(); // One-time setup
    
    // GLUT Callbacks
    void display();
    void reshape(int w, int h);
    void keyboardSpecial(int key, int x, int y); // For arrow keys
    void keyboardAscii(unsigned char key, int x, int y); // For 'p', 'r', Space
    void update(); // Main game loop

private:
    void drawPlayer();
    void setupCamera();

    // --- Game Logic Functions ---
    void restart();
    void checkCollisions();
    void updateWorld(); // For procedural generation
    void drawHUD();     // For drawing score and messages
    void drawText(std::string text, float x, float y, void* font);

    // --- Menu-specific functions ---
    void setupMenuCamera();
    void drawMenuScene();

    // --- Game State ---
    int playerX;
    int playerZ;
    int score;
    int maxZ; // Farthest Z player has reached

    GameState currentState; // To track game state

    // --- NEW: Difficulty Settings ---
    int selectedDifficulty; // 0=Easy, 1=Medium, 2=Hard
    float difficultyMultiplier;

    float cameraY;
    float cameraZ_offset;

    // Use a deque for efficient adding/removing from both ends
    std::deque<Lane*> lanes;
};