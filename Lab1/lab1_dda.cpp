#include <GL/glut.h> 
#include <math.h>

void drawLine(float x1, float y1, float x2, float y2)
{
    float dx = x2 - x1;
    float dy = y2 - y1;
    float steps = (fabs(dx) > fabs(dy)) ? fabs(dx) : fabs(dy);
    float xIncrement = dx / steps;
    float yIncrement = dy / steps;
    float x = x1, y = y1;
    glBegin(GL_POINTS);
    for (int i = 0; i <= steps; i++)
    {
        glVertex2i(round(x), round(y));
        x += xIncrement;
        y += yIncrement;
    }
    glEnd();
    glFlush();
}

void display_dda()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0f, 0.0f, 0.0f); // Set line color to red
    drawLine(100, 50, 200, 350); // Draw a line from (50, 50) to (200, 200)
}

int dda_main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500, 500); // Here we set the window size
    glutCreateWindow("DDA Line Drawing");
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black
    glMatrixMode(GL_PROJECTION); // Switch to projection matrix
    glLoadIdentity(); // Reset the matrix
    gluOrtho2D(0, 500, 0, 500); // Set orthographic projection
    glutDisplayFunc(display_dda);
    glutMainLoop();
    return 0;
}