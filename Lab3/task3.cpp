#include <GL/glut.h>

void drawParabolaPoints(int x, int y, int xc, int yc)
{
    glBegin(GL_POINTS);
    glVertex2i(xc + x, yc + y);
    glVertex2i(xc + x, yc - y);
    glEnd();
}

void drawParabola(int xc, int yc, int a, bool show_diff_color = false)
{
    int x = 0, y = 0;
    int p = 1 - 2 * a;

    if (show_diff_color)
    {
        glColor3f(1.0f, 1.0f, 0.0f);
    }

    // Region 1: |dy/dx| <= 1
    while (y < 2 * a)
    {

        drawParabolaPoints(x, y, xc, yc);

        if (p < 0)
        {
            y++;
            p += 2 * y + 1;
        }
        else
        {
            x++;
            y++;
            p += 2 * y + 1 - 4 * a;
        }
    }

    if (show_diff_color)
    {
        glColor3f(0.0f, 1.0f, 1.0f);
    }

    // Region 2: |dy/dx| > 1
    p = (y + 0.5) * (y + 0.5) - 4 * a * (x + 1);
    while (x < 200)
    {
        drawParabolaPoints(x, y, xc, yc);

        if (p > 0)
        {
            x++;
            p += 1 - 4 * a;
        }
        else
        {
            x++;
            y++;
            p += 2 * y + 1 - 4 * a;
        }
    }
    glFlush();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Drawing Parabola
    glColor3f(1.0f, 0.0f, 0.0f);
    drawParabola(10, 200, 70);
    drawParabola(50, 200, 40, true);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500, 500); // Here we set the window size
    glutCreateWindow("Task 3");
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black
    glMatrixMode(GL_PROJECTION);          // Switch to projection matrix
    glLoadIdentity();                     // Reset the matrix
    gluOrtho2D(0, 500, 500, 0);           // Set orthographic projection
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}