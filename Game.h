#pragma once

#include <vector>
#include "Lane.h"

// Game class manages all game state and logic
class Game {
public:
    Game(); // Constructor
    void init(); // One-time setup
    
    // GLUT Callbacks
    void display();
    void reshape(int w, int h);
    void keyboard(int key, int x, int y);
    void update(); // Main game loop

private:
    void drawPlayer();
    void setupCamera();

    // --- Game State ---
    int playerX;
    int playerZ;

    float cameraY;
    float cameraZ_offset;

    // A list of all the lanes in our world
    std::vector<Lane*> lanes;
};