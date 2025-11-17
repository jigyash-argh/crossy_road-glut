#include<GL/glut.h>
int main(int argc,char**argv){
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE);
    glutInitWindowSize(800,600);
    glutCreateWindow("basics ");
    glutMainLoop();
}