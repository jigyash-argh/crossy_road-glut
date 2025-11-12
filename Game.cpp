#include "Game.h"
#include <GL/glut.h>
#include <cstdlib> // For srand, rand
#include <ctime>   // For time

Game::Game() {
    // Initialize variables
    playerX = 0;
    playerZ = 0;
    cameraY = 10.0f;
    cameraZ_offset = 5.0f;
}

void Game::init() {
    // Set the "clear" color to a light blue (sky color)
    glClearColor(0.7f, 0.8f, 1.0f, 1.0f);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    
    // Enable lighting and a single light source
    // This makes the 3D shapes look much better
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL); // Allows glColor to work with lighting
    
    // Set light properties
    GLfloat light_pos[] = { 1.0f, 1.0f, 1.0f, 0.0f }; // Directional light
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

    // Seed the random number generator
    srand((unsigned int)time(NULL));

    // --- Create a static world ---
    // This is the next step from your old code.
    // Later, we will make this "procedural" (infinite).
    
    // Starting safe zone
    lanes.push_back(new Lane(LANE_GRASS, 0));
    lanes.push_back(new Lane(LANE_GRASS, -1));
    lanes.push_back(new Lane(LANE_GRASS, -2));

    // Add 20 random lanes
    for (int z = -3; z > -23; z--) {
        // 1 in 3 chance of being a grass lane
        LaneType type = (rand() % 3 == 0) ? LANE_GRASS : LANE_ROAD;
        lanes.push_back(new Lane(type, z));
    }
}

void Game::setupCamera() {
    // Set up the camera using gluLookAt
    gluLookAt(
        (float)playerX, cameraY, (float)playerZ + cameraZ_offset, // Eye Position
        (float)playerX, 0.0, (float)playerZ,                      // LookAt Position
        0.0, 1.0, 0.0                                             // Up Vector
    );
}

void Game::display() {
    // 1. Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 2. Set up the ModelView matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // 3. Position the camera
    setupCamera();

    // 4. Draw all the lanes
    // This will also draw the cars/busses in each lane
    for (Lane* lane : lanes) {
        lane->draw();
    }

    // 5. Draw the player
    drawPlayer();

    // 6. Swap the buffers
    glutSwapBuffers();
}

void Game::reshape(int w, int h) {
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (double)w / (double)h, 1.0f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

void Game::keyboard(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_UP:
        playerZ--; // Move forward
        break;
    case GLUT_KEY_DOWN:
        playerZ++; // Move backward
        break;
    case GLUT_KEY_LEFT:
        playerX--; // Move left
        break;
    case GLUT_KEY_RIGHT:
        playerX++; // Move right
        break;
    }
    // We don't call glutPostRedisplay() here anymore!
    // The update() loop handles redrawing.
}

/**
 * @brief This is the new Game Loop!
 */
void Game::update() {
    // This is where all animation and game logic happens.
    
    // 1. Update all lanes (which in turn updates all obstacles)
    for (Lane* lane : lanes) {
        lane->update();
    }

    // 2. TODO: Check for collisions
    // We will add this in the next step!
    // e.g., checkCollisions();

    // 3. Force a redraw of the screen
    glutPostRedisplay();
}

void Game::drawPlayer() {
    glPushMatrix();
    
    // Set color to yellow
    // Note: glColor3f works because we enabled GL_COLOR_MATERIAL
    glColor3f(1.0f, 1.0f, 0.0f); 
    
    // Position the player
    glTranslatef((float)playerX, 0.5f, (float)playerZ + 0.5f);

    // Draw a cube
    glutSolidCube(1.0f);

    glPopMatrix();
}