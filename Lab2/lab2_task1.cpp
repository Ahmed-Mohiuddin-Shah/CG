#include <GL/freeglut.h>

// Bresenham’s Line Algorithm
void drawLine(int x1, int y1, int x2, int y2)
{
    int dx = abs(x2 - x1), dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    glBegin(GL_POINTS);
    while (x1 != x2 || y1 != y2)
    {
        glVertex2i(x1, y1);
        int e2 = 2 * err;
        if (e2 > -dy)
        {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y1 += sy;
        }
    }
    glEnd();
    glFlush();
}

void drawRectangle(int top_left_x, int top_left_y, int bottom_right_x, int bottom_right_y)
{
    if (top_left_x < 0 || top_left_y < 0 || bottom_right_x < 0 || bottom_right_y < 0)
    {
        // coordinates shouldn't be negative
        return;
    }

    // top line
    drawLine(top_left_x, top_left_y, top_left_x + bottom_right_x, top_left_y);

    drawLine(top_left_x + bottom_right_x, top_left_y, top_left_x + bottom_right_x, top_left_y + bottom_right_y);

    drawLine(top_left_x + bottom_right_x, top_left_y + bottom_right_y, top_left_x, top_left_y + bottom_right_y);

    drawLine(top_left_x, top_left_y, top_left_x, top_left_y + bottom_right_y);
}

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

    // Drawing Green Rectangles
    glColor3f(0.0f, 1.0f, 0.0f); // Set Line Color to green
    drawRectangle(10, 10, 300, 250);
    drawRectangle(20, 20, 50, 40);

    // Drawing red Circles
    glColor3f(1.0f, 0.0f, 0.0f);
    drawCircle(400, 400, 100);
    drawCircle(200, 400, 60);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(700, 700); // Here we set the window size
    glutCreateWindow("Task 1");
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black
    glMatrixMode(GL_PROJECTION);          // Switch to projection matrix
    glLoadIdentity();                     // Reset the matrix
    gluOrtho2D(0, 700, 700, 0);           // Set orthographic projection
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}