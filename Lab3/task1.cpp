#include <GL/freeglut.h>

void drawCirclePoints(int x, int y, int center_x, int center_y)
{

    // We used the 8 point symmetry of the circle to save on number of calculations

    glBegin(GL_POINTS);
    glVertex2i(center_x + x, center_y + y);
    glVertex2i(center_x - x, center_y + y);
    glVertex2i(center_x + x, center_y - y);
    glVertex2i(center_x - x, center_y - y);
    glVertex2i(center_x + y, center_y + x);
    glVertex2i(center_x - y, center_y + x);
    glVertex2i(center_x + y, center_y - x);
    glVertex2i(center_x - y, center_y - x);
    glEnd();
}

void drawCircle(int center_x, int center_y, int radius)

{
    int x = 0, y = radius;
    int p = 1 - radius; // Midpoint decision parameter

    drawCirclePoints(x, y, center_x, center_y);
    while (x < y)
    {
        x++;
        if (p < 0)
            p += 2 * x + 1;
        else
        {
            y--;
            p += 2 * x - 2 * y + 1;
        }
        drawCirclePoints(x, y, center_x, center_y);
    }
    glFlush();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Drawing red Circles
    glColor3f(1.0f, 0.0f, 0.0f);
    drawCircle(280, 280, 100);
    glColor3f(1.0f, 1.0f, 0.0f);
    drawCircle(120, 120, 80);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500, 500); // Here we set the window size
    glutCreateWindow("Task 1");
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black
    glMatrixMode(GL_PROJECTION);          // Switch to projection matrix
    glLoadIdentity();                     // Reset the matrix
    gluOrtho2D(0, 500, 500, 0);           // Set orthographic projection
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}