#include <GL/freeglut.h>
#include <math.h>
#define PI 3.14159265358979323846

// DDA Algorithm for Line Drawing
void drawLineDDA(float x1, float y1, float x2, float y2)
{
    float dx = x2 - x1;
    float dy = y2 - y1;
    float steps = (abs(dx) > abs(dy)) ? abs(dx) : abs(dy);
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

void drawStarPolygonal(float cx, float cy, float radius)
{
    const int numPoints = 5;          // 5-pointed star
    float angleStep = PI / numPoints; // 36 degrees per step
    float innerRadius = radius / 2.5; // Smaller radius for inner points
    float points[10][2];

    // Compute the outer and inner points
    for (int i = 0; i < 10; i++)
    {
        float r = (i % 2 == 0) ? radius : innerRadius;           // Alternate between outer and inner radius
        points[i][0] = cx + r * cos(i * angleStep * 2 - PI / 2); // Adjusted angle for correct orientation
        points[i][1] = cy + r * sin(i * angleStep * 2 - PI / 2);
    }
    // Correct star connections (every second point)
    for (int i = 0; i < 10; i++)
    {
        drawLineDDA(points[i][0], points[i][1], points[(i + 1) % 10][0], points[(i + 1) % 10][1]);
    }
}

void drawStarCircle(int N, float cx, float cy, float radius)
{
    float points[N][2];           // Store coordinates of N points
    float angleStep = 2 * PI / N; // Angle step between points

    // Compute N equidistant points on the circle
    for (int i = 0; i < N; i++)
    {
        points[i][0] = cx + radius * cos(i * angleStep);
        points[i][1] = cy + radius * sin(i * angleStep);
    }

    // Connect every non-adjacent pair
    for (int i = 0; i < N; i++)
    {
        for (int j = i + 2; j < N; j++)
        { // Skip adjacent points (i+1 and i-1)
            if (j != (i - 1 + N) % N)
            { // Avoid connecting last point to first
                drawLineDDA(points[i][0], points[i][1], points[j][0], points[j][1]);
            }
        }
    }
}

void drawPolygon(int sides, float cx, float cy, float radius)
{
    float angleStep = 2 * PI / sides;
    float points[sides][2];

    for (int i = 0; i < sides; i++)
    {
        points[i][0] = cx + radius * cos(i * angleStep);
        points[i][1] = cy + radius * sin(i * angleStep);
    }

    for (int i = 0; i < sides; i++)
    {
        drawLineDDA(points[i][0], points[i][1], points[(i + 1) % sides][0], points[(i + 1) % sides][1]);
    }
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Drawing Green Polygons
    glColor3f(0.0f, 1.0f, 0.0f);  // Set Line Color to green
    drawPolygon(5, 70, 70, 60);   // Five Sided
    drawPolygon(6, 300, 300, 50); // Six Sided

    // Drawing red Star
    glColor3f(1.0f, 0.0f, 0.0f);
    drawStarPolygonal(400, 500, 100); // Five Point Star
    drawStarCircle(5, 200, 400, 50);  // Five Point star using circle method
    drawStarCircle(6, 70, 500, 60);  // Six Point star using circle method
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(700, 700); // Here we set the window size
    glutCreateWindow("Task 2");
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black
    glMatrixMode(GL_PROJECTION);          // Switch to projection matrix
    glLoadIdentity();                     // Reset the matrix
    gluOrtho2D(0, 700, 700, 0);           // Set orthographic projection
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}