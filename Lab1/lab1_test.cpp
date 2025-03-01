#include <GL/glut.h>

void display_test()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
}

int test_main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("OpenGL Test");
    glutDisplayFunc(display_test);
    glutMainLoop();
    return 0;
}