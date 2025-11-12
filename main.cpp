
#include <GL/glut.h>
#include "Game.h"

// --- Global Game Object ---
Game myGame;

// --- GLUT Callback Wrappers ---

void displayWrapper() {
    myGame.display();
}

void reshapeWrapper(int w, int h) {
    myGame.reshape(w, h);
}

void keyboardSpecialWrapper(int key, int x, int y) {
    myGame.keyboardSpecial(key, x, y);
}

/**
 * @brief NEW: Wrapper for ASCII keys like 'p' and 'r'
 */
void keyboardAsciiWrapper(unsigned char key, int x, int y) {
    myGame.keyboardAscii(key, x, y);
}

void updateWrapper(int value) {
    myGame.update(); // Call the game's update logic
    glutTimerFunc(16, updateWrapper, 0); // 16ms ~= 60 FPS
}

// --- Main Function ---

int main(int argc, char** argv) {
    // 1. Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1024, 768); // Made window a bit bigger
    glutCreateWindow("Crossy Road - Full Game");

    // 2. Call our one-time init function
    myGame.init();

    // 3. Register our callback functions
    glutDisplayFunc(displayWrapper);
    glutReshapeFunc(reshapeWrapper);
    glutSpecialFunc(keyboardSpecialWrapper); // For arrow keys
    glutKeyboardFunc(keyboardAsciiWrapper);  // NEW: For 'p' and 'r'

    // 4. Register the update timer
    glutTimerFunc(16, updateWrapper, 0);

    // 5. Start the main GLUT loop
    glutMainLoop();

    return 0;
}