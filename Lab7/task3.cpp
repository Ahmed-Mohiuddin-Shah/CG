#include <GL/glut.h>
#include <math.h>
#include <vector>
#include <iostream>
#include "helpers.h"

// // Struct definitions
// struct Point
// {
//     int x, y;
// };

// struct Line
// {
//     int x1, y1, x2, y2;
// };

// struct PolygonShape
// {
//     std::vector<Point> points;
// };

// struct ViewPort
// {
//     int x, y, width, height;
// };

// struct Params
// {
//     ViewPort viewPort;
//     std::vector<Line> lines;
//     std::vector<PolygonShape> polygons;
// };

// Global variables
Params params;
int screenWidth = 800;
int screenHeight = 800;
bool toggle = false;
bool isDrawing = false;
bool lineDrawingMode = false; // Tracks if we are in line-drawing mode
Point tempPoint;
Line tempLine;
PolygonShape tempPolygon;

// Function prototypes
void display();
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void drawViewPort(ViewPort *viewPort, int screenWidth, int screenHeight);
void drawCohenSutherlandLineClip(Line line, ViewPort viewPort);
void drawSutherHodgmanPolygonClip(PolygonShape *polygon, ViewPort viewPort);

// Display function
void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    drawViewPort(&params.viewPort, screenWidth, screenHeight);

    // Draw lines
    for (auto line : params.lines)
    {
        glColor3f(1.0, 0.0, 0.0);
        if (toggle)
        {
            // Draw original line
            glBegin(GL_LINES);
            glVertex2i(line.x1, line.y1);
            glVertex2i(line.x2, line.y2);
            glEnd();
            // Label endpoints
            glRasterPos2i(line.x1 + 10, line.y1 - 20);
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'A');
            glRasterPos2i(line.x2 + 10, line.y2 - 20);
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'B');
        }
        else
        {
            // Draw clipped line
            drawCohenSutherlandLineClip(line, params.viewPort);
        }
    }

    // Draw polygons
    for (auto polygon : params.polygons)
    {
        glColor3f(1.0, 0.0, 0.0);
        if (toggle)
        {
            // Draw original polygon
            for (int i = 0; i < polygon.points.size(); i++)
            {
                glBegin(GL_LINES);
                glVertex2i(polygon.points[i].x, polygon.points[i].y);
                glVertex2i(polygon.points[(i + 1) % polygon.points.size()].x, polygon.points[(i + 1) % polygon.points.size()].y);
                glEnd();
                // Label vertices
                glRasterPos2i(polygon.points[i].x + 10, polygon.points[i].y - 20);
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'A' + i);
            }
        }
        else
        {
            // Draw clipped polygon
            drawSutherHodgmanPolygonClip(&polygon, params.viewPort);
        }
    }

    glFlush();
}

// Keyboard function
void keyboard(unsigned char key, int x, int y)
{
    if (key == 27) // ESC key
        exit(0);
    if (key == 113)
    {                            // 'q' key{
        params.polygons.clear(); // Clear polygons
        params.lines.clear();
    }                                       // Clear lines}
    if (key == 119)                         // 'w' key
        toggle = !toggle;                   // Toggle between original and clipped shapes
    if (key == 108)                         // 'l' key
        lineDrawingMode = !lineDrawingMode; // Toggle line-drawing mode

    glutPostRedisplay(); // Rerender
}

// Mouse function
void mouse(int button, int state, int x, int y)
{
    y = screenHeight - y; // Convert y-coordinate to OpenGL coordinate system

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        if (lineDrawingMode)
        {
            // Line-drawing mode: First click sets start point, second click sets end point
            if (!isDrawing)
            {
                tempLine = {x, y, x, y}; // Initialize line
                isDrawing = true;
            }
            else
            {
                tempLine.x2 = x;
                tempLine.y2 = y;
                params.lines.push_back(tempLine); // Add line to the list
                isDrawing = false;
            }
        }
        else
        {
            // Polygon-drawing mode
            isDrawing = true;
            tempPoint = {x, y};
            tempPolygon.points.push_back({x, y});
        }
    }

    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP && !lineDrawingMode)
    {
        isDrawing = false;
        if (tempPolygon.points.size() > 2)
        {
            params.polygons.push_back(tempPolygon); // Add polygon to the list
        }
    }
}

// Motion function (for drawing)
void motion(int x, int y)
{
    y = screenHeight - y; // Convert y-coordinate to OpenGL coordinate system

    if (isDrawing && !lineDrawingMode)
    {
        tempPolygon.points.push_back({x, y});
        glutPostRedisplay();
    }
}

// Main function
int main(int argc, char **argv)
{
    params = getParams(true, true);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(screenWidth, screenHeight);
    glutCreateWindow("Task3 - Combined Clipping");
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, 800.0, 0.0, 800.0);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutMainLoop();
    return 0;
}