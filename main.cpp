
#include <GL/glut.h>
#include "Game.h"
Game myGame;

void displayWrapper() {
    myGame.display();
}

void reshapeWrapper(int w, int h) {
    myGame.reshape(w, h);
}

void keyboardSpecialWrapper(int key, int x, int y) {
    myGame.keyboardSpecial(key, x, y);
}

void keyboardAsciiWrapper(unsigned char key, int x, int y) {
    myGame.keyboardAscii(key, x, y);
}

void updateWrapper(int value) {
    myGame.update(); 
    glutTimerFunc(16, updateWrapper, 0); // 16ms ~= 60 FPS
}


int main(int argc, char** argv) {
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

    glutTimerFunc(16, updateWrapper, 0);

    glutMainLoop();

    return 0;
}