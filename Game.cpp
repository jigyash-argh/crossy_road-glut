#include "Game.h"
#include <GL/glut.h>
#include <cstdlib>      
#include <ctime>        
#include <string>       
#include <sstream>      

const int STARTING_LANES = 10;
const int LANES_AHEAD = 15;     
const int LANES_BEHIND = 5;      

Game::Game() {
    init(); 
    currentState = STATE_MENU;
}


void Game::init() {

    for (Lane* lane : lanes) {
        delete lane;
    }
    lanes.clear();

    playerX = 0;
    playerZ = 0;
    score = 0;
    maxZ = 0;
    // currentState = STATE_PLAYING; // <-- THIS LINE IS REMOVED
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

/**
 * @brief MODIFIED: restart()
 * This function is now used to *start* or *restart* the game.
 * It calls init() and then sets the state to PLAYING.
 */
void Game::restart() {
    init(); // Reset all variables
    currentState = STATE_PLAYING; // Set the state to playing
}

void Game::setupCamera() {
    gluLookAt(
        (float)playerX, cameraY, (float)playerZ + cameraZ_offset,
        (float)playerX, 0.0, (float)playerZ,
        0.0, 1.0, 0.0
    );
}

/**
 * @brief NEW: setupMenuCamera()
 * Sets up a static camera for the main menu, looking at the origin.
 */
void Game::setupMenuCamera() {
    gluLookAt(
        -4.0f, 6.0f, 6.0f,  // Eye position (from the side)
        0.0f, 0.0f, 0.0f,   // Look at origin
        0.0f, 1.0f, 0.0f    // Up vector
    );
}

/**
 * @brief MODIFIED: display()
 * Now checks the state. If on menu, draws the menu.
 * Otherwise, draws the game.
 */
void Game::display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // --- Check Game State ---
    if (currentState == STATE_MENU) {
        setupMenuCamera();
        drawMenuScene();
    } else {
        // We are PLAYING, PAUSED, or GAME_OVER
        setupCamera();

        // Draw all the lanes and obstacles
        for (Lane* lane : lanes) {
            lane->draw();
        }

        // Draw the player
        drawPlayer();
    }
    
    // Draw the Score, Pause, and Game Over messages
    // This is called in all states
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

/**
 * @brief MODIFIED: keyboardAscii()
 * Added 'Spacebar' (32) and 'Enter' (13) to start the game
 * from the menu.
 */
void Game::keyboardAscii(unsigned char key, int x, int y) {
    switch (key) {
    case ' ':  // Spacebar
    case 13:   // Enter key
        if (currentState == STATE_MENU) {
            restart(); // This starts the game
        }
        break;

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

    // Always force a redraw, even if paused or on menu
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

            return; // Found the lane, stop searching
        }
    }
}

void Game::updateWorld() {
    // --- Procedural Generation ---
    
    // 1. Add new lanes in front
    while (playerZ < lanes.front()->getZ() + LANES_AHEAD) {
        int newZ = lanes.front()->getZ() - 1;
        LaneType type = (rand() % 3 == 0) ? LANE_GRASS : LANE_ROAD;
        lanes.push_front(new Lane(type, newZ));
    }

    // 2. Remove old lanes from behind
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
 * @brief NEW: drawMenuScene()
 * Draws the simple 3D scene for the main menu.
 */
void Game::drawMenuScene() {
    // 1. Draw a ground (grass)
    glPushMatrix();
    glColor3f(0.0f, 0.6f, 0.2f);
    glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3f(-10.0f, 0.0f, -10.0f);
        glVertex3f(10.0f, 0.0f, -10.0f);
        glVertex3f(10.0f, 0.0f, 10.0f);
        glVertex3f(-10.0f, 0.0f, 10.0f);
    glEnd();
    glPopMatrix();

    // 2. Draw a "player" model
    glPushMatrix();
    glColor3f(1.0f, 1.0f, 0.0f); // Yellow
    glTranslatef(0.0f, 0.5f, 2.0f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // 3. Draw a "car" model
    glPushMatrix();
    glColor3f(1.0f, 0.0f, 0.0f); // Red car
    glTranslatef(-2.0f, 0.5f, -1.0f);
    glScalef(1.5f, 1.0f, 1.0f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // 4. Draw a "bus" model
    glPushMatrix();
    glColor3f(0.0f, 0.0f, 0.8f); // Blue bus
    glTranslatef(2.5f, 0.75f, -1.0f);
    glScalef(2.5f, 1.5f, 1.0f);
    glutSolidCube(1.0f);
    glPopMatrix();
}


/**
 * @brief MODIFIED: drawHUD()
 * Now includes a state for STATE_MENU to draw the title
 */
void Game::drawHUD() {
    // --- Build Score String ---
    std::stringstream ss;
    ss << "Score: " << score;

    // --- Draw Score (only if playing) ---
    if (currentState == STATE_PLAYING) {
        glColor3f(1.0f, 1.0f, 1.0f); // White text
        drawText(ss.str(), 10.0f, 10.0f, GLUT_BITMAP_HELVETICA_18);
    }
    
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
    else if (currentState == STATE_MENU) {
        // --- NEW: Draw the Main Menu ---
        glColor3f(1.0f, 1.0f, 1.0f); // White
        drawText("CROSSY ROAD", 400.0f, 200.0f, GLUT_BITMAP_TIMES_ROMAN_24);
        
        glColor3f(1.0f, 1.0f, 0.0f); // Yellow
        drawText("Press 'Space' or 'Enter' to Start", 350.0f, 400.0f, GLUT_BITMAP_HELVETICA_18);
        
        glColor3f(0.8f, 0.8f, 0.8f); // Light gray
        drawText("--- Controls ---", 430.0f, 450.0f, GLUT_BITMAP_HELVETICA_18);
        drawText("Arrow Keys: Move", 430.0f, 470.0f, GLUT_BITMAP_HELVETICA_18);
        drawText("P: Pause", 430.0f, 490.0f, GLUT_BITMAP_HELVETICA_18);
        drawText("R: Restart (when paused/game over)", 350.0f, 510.0f, GLUT_BITMAP_HELVETICA_18);
    }
}

void Game::drawText(std::string text, float x, float y, void* font) {

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, 1024.0, 768.0, 0.0); 

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(font, c);
    }

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}