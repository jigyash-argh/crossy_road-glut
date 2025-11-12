/**
 * Crossy Road - Starter Project
 *
 * This file covers Step 1 (World & Camera) and Step 2 (Player Control).
 *
 * --- How to Compile (on Linux/macOS with freeglut) ---
 * g++ -o crossy_road crossy_road.cpp -lglut -lGLU -lGL
 *
 * --- How to Compile (on Windows with VS Code) ---
 * This assumes you have set up freeglut. You will need to link the
 * appropriate libraries in your VS Code build task (tasks.json).
 * Your linker flags will need: -lfreeglut -lopengl32 -lglu32
 *
 * --- Controls ---
 * Up Arrow:    Move player forward (negative Z)
 * Down Arrow:  Move player backward (positive Z)
 * Left Arrow:  Move player left (negative X)
 * Right Arrow: Move player right (positive X)
 */

// If you are using Windows, you might need <windows.h> and <GL/glut.h>
// On macOS or Linux, <GL/glut.h> or <GL/freeglut.h> is common.
#include <GL/glut.h>
#include <iostream> // For std::cout

// --- Global Variables (Game State) ---

// Player's grid position.
// We use 'int' because the player moves one full tile at a time.
int playerX = 0;
int playerZ = 0;

// Camera's position relative to the player.
// We'll keep the camera 10 units up and 5 units behind the player.
float cameraY = 10.0f;
float cameraZ_offset = 5.0f;

// --- Helper Functions ---

/**
 * @brief Initializes OpenGL states.
 */
void init() {
    // Set the "clear" color to a light blue (sky color)
    glClearColor(0.7f, 0.8f, 1.0f, 1.0f);

    // Enable depth testing. This is ESSENTIAL for 3D.
    // It makes objects in front correctly cover objects behind them.
    glEnable(GL_DEPTH_TEST);
}

/**
 * @brief Draws the player (a simple yellow cube)
 * at its current grid position.
 */
void drawPlayer() {
    // glPush/PopMatrix ensures that our transformations (translate, color)
    // don't affect other objects in the scene.
    glPushMatrix();

    // Set the color to yellow
    glColor3f(1.0f, 1.0f, 0.0f); // R, G, B

    // Translate the cube to the player's (x, z) position.
    // We lift it by 0.5f in the Y-axis so it sits ON TOP of the
    // ground plane (which is at y=0).
    glTranslatef((float)playerX, 0.5f, (float)playerZ);

    // Draw a solid cube of size 1x1x1
    glutSolidCube(1.0f);

    glPopMatrix();
}

/**
 * @brief Draws the "world" (the lanes).
 * For now, this is just a static set of striped quads.
 */
void drawLanes() {
    // This is a simple, static world.
    // In your next step, you'll replace this with a std::vector<Lane>
    // and draw each Lane object.
    for (int z = -50; z < 50; z++) {
        // We'll make every 3rd lane a "road" (gray)
        // and the others "grass" (green).
        if (z % 3 == 0) {
            glColor3f(0.2f, 0.2f, 0.2f); // Dark Gray (Road)
        } else {
            glColor3f(0.0f, 0.6f, 0.2f); // Green (Grass)
        }

        // Draw a 1-unit wide quad for this lane.
        // We make it wide in the X-direction.
        glBegin(GL_QUADS);
        glVertex3f(-20.0f, 0.0f, (float)z);       // Bottom-left
        glVertex3f(20.0f, 0.0f, (float)z);       // Bottom-right
        glVertex3f(20.0f, 0.0f, (float)z + 1.0f); // Top-right
        glVertex3f(-20.0f, 0.0f, (float)z + 1.0f); // Top-left
        glEnd();
    }
}

// --- GLUT Callback Functions ---

/**
 * @brief The main display function.
 * This is called whenever the screen needs to be redrawn.
 */
void display() {
    // 1. Clear the screen's color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 2. Set up the ModelView matrix
    // This is where we place our "camera"
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); // Reset the matrix

    // Set up the camera using gluLookAt
    // This is the core of our 3/4 perspective!
    gluLookAt(
        (float)playerX, cameraY, (float)playerZ + cameraZ_offset, // Eye (Camera) Position:
                                                                 // X follows player, Y is 10 units up, Z is 5 units behind player.

        (float)playerX, 0.0, (float)playerZ, // Center (LookAt) Position:
                                             // We are always looking AT the player's grid position (at y=0).

        0.0, 1.0, 0.0  // Up Vector:
                       // Tells OpenGL that "up" is in the positive Y direction.
    );

    // 3. Draw our game objects
    drawLanes();
    drawPlayer();

    // 4. Swap the buffers
    // This shows the frame we just drew and hides the one we were
    // drawing on (this is "double buffering").
    glutSwapBuffers();
}

/**
 * @brief Called when the window is resized.
 * This is where we set up our 3D projection.
 */
void reshape(int w, int h) {
    // Set the viewport to cover the new window size
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);

    // Set the matrix mode to Projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity(); // Reset the projection matrix

    // Set up the perspective projection
    // 45.0f = Field of View (angle)
    // (double)w / (double)h = Aspect Ratio
    // 1.0f = Near clipping plane (don't draw things closer than 1 unit)
    // 100.0f = Far clipping plane (don't draw things farther than 100 units)
    gluPerspective(45.0f, (double)w / (double)h, 1.0f, 100.0f);

    // Switch back to ModelView matrix mode
    glMatrixMode(GL_MODELVIEW);
}

/**
 * @brief Handles special key presses (like arrow keys).
 */
void keyboard(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_UP:
        playerZ--; // Move "into" the screen (negative Z)
        break;
    case GLUT_KEY_DOWN:
        playerZ++; // Move "out of" the screen (positive Z)
        break;
    case GLUT_KEY_LEFT:
        playerX--; // Move left
        break;
    case GLUT_KEY_RIGHT:
        playerX++; // Move right
        break;
    }

    // After moving, we must tell GLUT to redraw the screen
    glutPostRedisplay();
}

// --- Main Function ---

/**
 * @brief The main entry point for the application.
 */
int main(int argc, char** argv) {
    // 1. Initialize GLUT
    glutInit(&argc, argv);

    // 2. Set display mode
    // GLUT_DOUBLE = Use double buffering
    // GLUT_RGB = Use RGB color
    // GLUT_DEPTH = Use a depth buffer (for 3D)
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    // 3. Set window size and create window
    glutInitWindowSize(800, 600);
    glutCreateWindow("Crossy Road - Step 1 & 2");

    // 4. Call our one-time init function
    init();

    // 5. Register our callback functions
    glutDisplayFunc(display);     // Set the main display function
    glutReshapeFunc(reshape);     // Set the reshape function
    glutSpecialFunc(keyboard);    // Set the special keyboard function

    // 6. Start the main GLUT loop
    // This function never returns.
    glutMainLoop();

    return 0; // Will never be reached
}