#pragma once

#include <deque> 
#include <string>
#include "Lane.h"

enum GameState {
    STATE_MENU,      
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_GAME_OVER
};

class Game {
public:
    Game(); 
    void init();
    void display();
    void reshape(int w, int h);
    void keyboardSpecial(int key, int x, int y); // For arrow keys
    void keyboardAscii(unsigned char key, int x, int y); // For 'p', 'r'
    void update(); 

private:
    void drawPlayer();
    void setupCamera();

    void restart();
    void checkCollisions();
    void updateWorld(); // For procedural generation
    void drawHUD();     // For drawing score and messages
    void drawText(std::string text, float x, float y, void* font);

    void setupMenuCamera();
    void drawMenuScene();

    int playerX;
    int playerZ;
    int score;
    int maxZ;

    GameState currentState;

    float cameraY;
    float cameraZ_offset;

    std::deque<Lane*> lanes;
};