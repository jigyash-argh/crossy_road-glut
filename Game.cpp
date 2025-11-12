#include "Game.h"
#include <GL/glut.h>
#include <cstdlib>      // For srand, rand
#include <ctime>        // For time
#include <string>       // For std::string
#include <sstream>      // For std::stringstream (to build strings)

// --- Constants ---
const int STARTING_LANES = 10;
const int LANES_AHEAD = 15;      // How many lanes to generate ahead of the player
const int LANES_BEHIND = 5;      // How many lanes to keep behind the player

Game::Game() {
    // Constructor (all initialization is now in init())
}

void Game::init() {
    // --- Clean up old lanes (for restarting) ---
    for (Lane* lane : lanes) {
        delete lane;
    }
    lanes.clear();

    // --- Reset Game State ---
    playerX = 0;
    playerZ = 0;
    score = 0;
    maxZ = 0;
    currentState = STATE_PLAYING;
    cameraY = 10.0f;
    cameraZ_offset = 5.0f;

    // --- Set up GL states ---
    glClearColor(0.7f, 0.8f, 1.0f, 1.0f); // Sky color
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    
    GLfloat light_pos[] = { 1.0f, 1.0f, 1.0f, 0.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

    // Seed the random number generator
    srand((unsigned int)time(NULL));

    // --- Create the starting world ---
    // Start with a safe grass zone
    for (int z = 1; z >= -STARTING_LANES; z--) {
        lanes.push_back(new Lane(LANE_GRASS, z));
    }
    
    // Add the first few random lanes
    for (int z = -STARTING_LANES - 1; z > -LANES_AHEAD; z--) {
        LaneType type = (rand() % 3 == 0) ? LANE_GRASS : LANE_ROAD;
        lanes.push_front(new Lane(type, z));
    }
}

void Game::restart() {
    // Simply call init() to reset everything
    init();
}

void Game::setupCamera() {
    gluLookAt(
        (float)playerX, cameraY, (float)playerZ + cameraZ_offset,
        (float)playerX, 0.0, (float)playerZ,
        0.0, 1.0, 0.0
    );
}

void Game::display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    setupCamera();

    // Draw all the lanes and obstacles
    for (Lane* lane : lanes) {
        lane->draw();
    }

    // Draw the player
    drawPlayer();
    
    // Draw the Score, Pause, and Game Over messages
    drawHUD();

    glutSwapBuffers();
}

void Game::reshape(int w, int h) {
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (double)w / (double)h, 1.0f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

void Game::keyboardSpecial(int key, int x, int y) {
    // --- Player can only move if the game is in PLAYING state ---
    if (currentState != STATE_PLAYING) {
        return;
    }

    switch (key) {
    case GLUT_KEY_UP:
        playerZ--;
        break;
    case GLUT_KEY_DOWN:
        playerZ++;
        break;
    case GLUT_KEY_LEFT:
        playerX--;
        break;
    case GLUT_KEY_RIGHT:
        playerX++;
        break;
    }

    // --- Update Score ---
    // Score is the farthest (most negative) Z value reached
    if (playerZ < maxZ) {
        maxZ = playerZ;
        score = abs(maxZ);
    }
}

void Game::keyboardAscii(unsigned char key, int x, int y) {
    switch (key) {
    case 'p': // Pause
    case 'P':
        if (currentState == STATE_PLAYING) {
            currentState = STATE_PAUSED;
        } else if (currentState == STATE_PAUSED) {
            currentState = STATE_PLAYING;
        }
        break;

    case 'r': // Restart
    case 'R':
        // Can only restart if paused or game is over
        if (currentState == STATE_PAUSED || currentState == STATE_GAME_OVER) {
            restart();
        }
        break;
    
    case 27: // Escape key
        exit(0);
        break;
    }
}

void Game::update() {
    // --- This is the main game loop ---
    
    // Only update animations and logic if the game is PLAYING
    if (currentState == STATE_PLAYING) {
        // 1. Update all lanes (which updates all obstacles)
        for (Lane* lane : lanes) {
            lane->update();
        }

        // 2. Check for collisions
        checkCollisions();

        // 3. Update the world (procedural generation)
        updateWorld();
    }

    // Always force a redraw, even if paused (to show menus)
    glutPostRedisplay();
}

void Game::checkCollisions() {
    // Find the lane the player is currently on
    for (Lane* lane : lanes) {
        if (lane->getZ() == playerZ) {
            
            // Check based on lane type
            if (lane->getLaneType() == LANE_ROAD) {
                // Check for collision with an obstacle
                if (lane->checkCollision(playerX)) {
                    currentState = STATE_GAME_OVER;
                }
            }
            // TODO: Add LANE_WATER logic
            // else if (lane->getLaneType() == LANE_WATER) {
            //    if (lane->checkCollision(playerX) == false) {
            //        // Player is not on a log!
            //        currentState = STATE_GAME_OVER;
            //    }
            // }

            return; // Found the lane, stop searching
        }
    }
}

void Game::updateWorld() {
    // --- Procedural Generation ---
    
    // 1. Add new lanes in front
    // (lanes.front() is the farthest lane, with most negative Z)
    while (playerZ < lanes.front()->getZ() + LANES_AHEAD) {
        int newZ = lanes.front()->getZ() - 1;
        LaneType type = (rand() % 3 == 0) ? LANE_GRASS : LANE_ROAD;
        lanes.push_front(new Lane(type, newZ));
    }

    // 2. Remove old lanes from behind
    // (lanes.back() is the closest lane)
    while (lanes.back()->getZ() > playerZ + LANES_BEHIND) {
        delete lanes.back();
        lanes.pop_back();
    }
}

void Game::drawPlayer() {
    glPushMatrix();
    
    // If game is over, make player flat and red
    if (currentState == STATE_GAME_OVER) {
        glColor3f(1.0f, 0.0f, 0.0f); // Red
        glTranslatef((float)playerX, 0.01f, (float)playerZ + 0.5f);
        glScalef(1.0f, 0.02f, 1.0f); // Flatten!
    } else {
        glColor3f(1.0f, 1.0f, 0.0f); // Yellow
        glTranslatef((float)playerX, 0.5f, (float)playerZ + 0.5f);
    }
    
    glutSolidCube(1.0f);
    glPopMatrix();
}

/**
 * @brief This new function draws all the 2D text on the screen
 */
void Game::drawHUD() {
    // --- Build Score String ---
    std::stringstream ss;
    ss << "Score: " << score;

    // --- Draw Score ---
    glColor3f(1.0f, 1.0f, 1.0f); // White text
    drawText(ss.str(), 10.0f, 10.0f, GLUT_BITMAP_HELVETICA_18);

    // --- Draw Game State Messages ---
    if (currentState == STATE_PAUSED) {
        glColor3f(1.0f, 1.0f, 0.0f); // Yellow
        drawText("PAUSED", 450.0f, 300.0f, GLUT_BITMAP_TIMES_ROMAN_24);
        drawText("Press 'P' to resume", 400.0f, 330.0f, GLUT_BITMAP_HELVETICA_18);
        drawText("Press 'R' to restart", 400.0f, 350.0f, GLUT_BITMAP_HELVETICA_18);
    }
    else if (currentState == STATE_GAME_OVER) {
        glColor3f(1.0f, 0.0f, 0.0f); // Red
        drawText("GAME OVER", 430.0f, 300.0f, GLUT_BITMAP_TIMES_ROMAN_24);
        
        std::stringstream finalScore;
        finalScore << "Final Score: " << score;
        drawText(finalScore.str(), 420.0f, 330.0f, GLUT_BITMAP_HELVETICA_18);
        
        drawText("Press 'R' to restart", 410.0f, 350.0f, GLUT_BITMAP_HELVETICA_18);
    }
}

/**
 * @brief Helper function to draw 2D text
 */
void Game::drawText(std::string text, float x, float y, void* font) {
    // --- Switch to 2D Orthographic Mode ---
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, 1024.0, 768.0, 0.0); // Match window size

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // --- Draw The Text ---
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(font, c);
    }

    // --- Restore 3D Mode ---
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}