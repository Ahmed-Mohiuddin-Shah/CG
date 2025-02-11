#include <GL/glut.h>
#include <vector>
#include <algorithm>

using namespace std;

struct Point
{
    int x, y;
};

// Global variables
vector<Point> polygon = {{100, 150}, {200, 50}, {300, 150}, {250, 300}, {150, 300}}; // Example polygon

void drawPolygon()
{
    glColor3f(0, 0, 0); // Black color for boundary
    glBegin(GL_LINE_LOOP);
    for (auto &p : polygon)
    {
        glVertex2i(p.x, p.y);
    }
    glEnd();
    glFlush();
}

void scanlineFill()
{
    int ymin = polygon[0].y, ymax = polygon[0].y;

    // Find ymin and ymax
    for (auto &p : polygon)
    {
        if (p.y < ymin)
            ymin = p.y;
        if (p.y > ymax)
            ymax = p.y;
    }

    for (int y = ymin; y <= ymax; y++)
    {
        vector<int> intersections;

        for (size_t i = 0; i < polygon.size(); i++)
        {
            Point p1 = polygon[i];
            Point p2 = polygon[(i + 1) % polygon.size()];

            if (p1.y != p2.y)
            { // Ignore horizontal edges
                if ((y >= p1.y && y < p2.y) || (y >= p2.y && y < p1.y))
                {
                    int x = p1.x + (y - p1.y) * (p2.x - p1.x) / (p2.y - p1.y);
                    intersections.push_back(x);
                }
            }
        }

        sort(intersections.begin(), intersections.end());

        glColor3f(1, 0, 0); // Fill color (red)
        glBegin(GL_LINES);
        for (size_t i = 0; i < intersections.size(); i += 2)
        {
            glVertex2i(intersections[i], y);
            glVertex2i(intersections[i + 1], y);
        }
        glEnd();
        glFlush();
    }
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    drawPolygon();
    scanlineFill();
}

void init()
{
    glClearColor(1, 1, 1, 1);
    gluOrtho2D(0, 500, 0, 500);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Task 4.3");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
