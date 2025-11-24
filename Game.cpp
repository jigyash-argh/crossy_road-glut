#include "Game.h"
#include "Constants.h"
#include <GL/glut.h>
#include <cstdlib>
#include <ctime>
#include <string>
#include <sstream>
#include <cmath>

// --- Constants ---
const int STARTING_LANES = 10;
const int LANES_AHEAD = 15;     
const int LANES_BEHIND = 5;      

/**
 * @brief MODIFIED: Constructor
 * Initializes new difficulty variables.
 */
Game::Game() {
    // --- NEW ---
    selectedDifficulty = 0; // 0=Easy, 1=Medium, 2=Hard
    difficultyMultiplier = 1.0f; // Default to Easy
    // --- ---

    init(); // Set up all variables
    currentState = STATE_MENU; // Start at the menu
}

/**
 * @brief MODIFIED: init()
 * Now uses the 'difficultyMultiplier' when creating the starting lanes.
 */
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
        // --- MODIFIED: Pass the multiplier ---
        lanes.push_back(new Lane(LANE_GRASS, z, difficultyMultiplier));
    }
    
    // Add the first few random lanes
    for (int z = -STARTING_LANES - 1; z > -LANES_AHEAD; z--) {
        LaneType type = (rand() % 3 == 0) ? LANE_GRASS : LANE_ROAD;
        // --- MODIFIED: Pass the multiplier ---
        lanes.push_front(new Lane(type, z, difficultyMultiplier));
    }
}

/**
 * @brief MODIFIED: restart()
 * This function is now used to *start* or *restart* the game.
 * It calls init() and then sets the state to PLAYING.
 */
void Game::restart() {
    init(); // Reset all variables (will use the chosen multiplier)
    currentState = STATE_PLAYING; // Set the state to playing
}

void Game::setupCamera() {
    // Wrap playerX into world range so camera stays on the repeated world
    float period = WORLD_HALF_WIDTH * 2.0f;
    float wrappedPlayerX = fmod(playerX + WORLD_HALF_WIDTH, period);
    if (wrappedPlayerX < 0) wrappedPlayerX += period;
    wrappedPlayerX -= WORLD_HALF_WIDTH;

    gluLookAt(
        wrappedPlayerX, cameraY, (float)playerZ + cameraZ_offset,
        wrappedPlayerX, 0.0, (float)playerZ,
        0.0, 1.0, 0.0
    );
}

void Game::setupMenuCamera() {
    gluLookAt(
        -4.0f, 6.0f, 6.0f,  // Eye position (from the side)
        0.0f, 0.0f, 0.0f,   // Look at origin
        0.0f, 1.0f, 0.0f    // Up vector
    );
}

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

/**
 * @brief MODIFIED: keyboardSpecial()
 * Now handles Up/Down arrows in the MENU state to change difficulty.
 * Handles player movement in the PLAYING state.
 */
void Game::keyboardSpecial(int key, int x, int y) {
    
    if (currentState == STATE_MENU) {
        // --- NEW: Handle menu selection ---
        switch (key) {
        case GLUT_KEY_UP:
            selectedDifficulty--;
            if (selectedDifficulty < 0) selectedDifficulty = 2;
            break;
        case GLUT_KEY_DOWN:
            selectedDifficulty++;
            if (selectedDifficulty > 2) selectedDifficulty = 0;
            break;
        }

    } else if (currentState == STATE_PLAYING) {
        // --- Player Movement ---
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
        if (playerZ < maxZ) {
            maxZ = playerZ;
            score = abs(maxZ);
        }
    }
}

/**
 * @brief MODIFIED: keyboardAscii()
 * When 'Space' or 'Enter' is pressed on the menu,
 * it now SETS the multiplier before starting the game.
 */
void Game::keyboardAscii(unsigned char key, int x, int y) {
    switch (key) {
    case ' ':  // Spacebar
    case 13:   // Enter key
        if (currentState == STATE_MENU) {
            
            // --- NEW: Set the multiplier based on selection ---
            if (selectedDifficulty == 0) { // Easy
                difficultyMultiplier = 1.0f;
            } else if (selectedDifficulty == 1) { // Medium
                difficultyMultiplier = 1.5f;
            } else { // Hard
                difficultyMultiplier = 2.0f;
            }
            
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
            // --- NEW: Reset difficulty to the one you chose ---
            // (This is subtle, if we just call restart()
            // it will use the already-set multiplier)
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

/**
 * @brief MODIFIED: updateWorld()
 * Now passes the 'difficultyMultiplier' when creating new lanes.
 */
void Game::updateWorld() {
    // --- Procedural Generation ---
    
    // 1. Add new lanes in front
    while (playerZ < lanes.front()->getZ() + LANES_AHEAD) {
        int newZ = lanes.front()->getZ() - 1;
        LaneType type = (rand() % 3 == 0) ? LANE_GRASS : LANE_ROAD;
        
        // --- MODIFIED: Pass the multiplier ---
        lanes.push_front(new Lane(type, newZ, difficultyMultiplier));
    }

    // 2. Remove old lanes from behind
    while (lanes.back()->getZ() > playerZ + LANES_BEHIND) {
        delete lanes.back();
        lanes.pop_back();
    }
}

void Game::drawPlayer() {
    // Draw a more detailed player composed of simple primitives
    float period = WORLD_HALF_WIDTH * 2.0f;
    float wrappedPlayerX = fmod(playerX + WORLD_HALF_WIDTH, period);
    if (wrappedPlayerX < 0) wrappedPlayerX += period;
    wrappedPlayerX -= WORLD_HALF_WIDTH;

    // Game over: flattened
    if (currentState == STATE_GAME_OVER) {
        glPushMatrix();
        glColor3f(1.0f, 0.0f, 0.0f);
        glTranslatef(wrappedPlayerX, 0.01f, (float)playerZ + 0.5f);
        glScalef(1.0f, 0.02f, 1.0f);
        glutSolidCube(1.0f);
        glPopMatrix();
        return;
    }

    // Normal player model
    glPushMatrix();
    glTranslatef(wrappedPlayerX, 0.0f, (float)playerZ + 0.5f);

    // Body (rounded cube)
    glPushMatrix();
    glTranslatef(0.0f, 0.45f, 0.0f);
    glColor3f(1.0f, 0.85f, 0.0f); // Warm yellow
    glScalef(0.9f, 0.6f, 0.9f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Head
    glPushMatrix();
    glTranslatef(0.0f, 0.95f, 0.0f);
    glColor3f(1.0f, 0.9f, 0.7f); // Skin tone
    glutSolidSphere(0.28f, 12, 12);

    // Eyes
    glPushMatrix();
    glColor3f(0.02f, 0.02f, 0.02f);
    glTranslatef(-0.09f, 0.02f, 0.25f);
    glutSolidSphere(0.04f, 8, 8);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.02f, 0.02f, 0.02f);
    glTranslatef(0.09f, 0.02f, 0.25f);
    glutSolidSphere(0.04f, 8, 8);
    glPopMatrix();

    glPopMatrix();

    // Legs (simple blocks)
    glPushMatrix();
    glTranslatef(-0.18f, 0.15f, 0.0f);
    glColor3f(0.2f, 0.2f, 0.7f); // Pants
    glPushMatrix();
    glScalef(0.25f, 0.3f, 0.4f);
    glutSolidCube(1.0f);
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.18f, 0.15f, 0.0f);
    glColor3f(0.2f, 0.2f, 0.7f);
    glPushMatrix();
    glScalef(0.25f, 0.3f, 0.4f);
    glutSolidCube(1.0f);
    glPopMatrix();
    glPopMatrix();

    glPopMatrix();
}

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
 * Now draws the difficulty selection on the main menu.
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
        // --- NEW: Draw the Main Menu with Difficulty ---
        glColor3f(1.0f, 1.0f, 1.0f); // White
        drawText("CROSSY ROAD", 400.0f, 200.0f, GLUT_BITMAP_TIMES_ROMAN_24);
        
        // --- Draw Difficulty Options ---
        glColor3f(0.8f, 0.8f, 0.8f);
        drawText("Use Arrow Keys to Select Difficulty:", 350.0f, 280.0f, GLUT_BITMAP_HELVETICA_18);

        // Draw "Easy"
        if (selectedDifficulty == 0) glColor3f(1.0f, 1.0f, 0.0f); // Yellow
        else glColor3f(1.0f, 1.0f, 1.0f); // White
        drawText("EASY", 460.0f, 320.0f, GLUT_BITMAP_HELVETICA_18);

        // Draw "Medium"
        if (selectedDifficulty == 1) glColor3f(1.0f, 1.0f, 0.0f); // Yellow
        else glColor3f(1.0f, 1.0f, 1.0f); // White
        drawText("MEDIUM", 460.0f, 350.0f, GLUT_BITMAP_HELVETICA_18);

        // Draw "Hard"
        if (selectedDifficulty == 2) glColor3f(1.0f, 1.0f, 0.0f); // Yellow
        else glColor3f(1.0f, 1.0f, 1.0f); // White
        drawText("HARD", 460.0f, 380.0f, GLUT_BITMAP_HELVETICA_18);

        // --- Draw Start Prompt ---
        glColor3f(1.0f, 1.0f, 0.0f); // Yellow
        drawText("Press 'Space' or 'Enter' to Start", 350.0f, 450.0f, GLUT_BITMAP_HELVETICA_18);
        
        // --- Draw Controls ---
        glColor3f(0.8f, 0.8f, 0.8f); // Light gray
        drawText("P: Pause", 450.0f, 500.0f, GLUT_BITMAP_HELVETICA_18);
        drawText("R: Restart", 450.0f, 520.0f, GLUT_BITMAP_HELVETICA_18);
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