//#include <GL/glut.h>
//#include <iostream>
//
//// Define boundary color (black) and fill color (red)
//float boundaryColor[3] = {0.0f, 0.0f, 0.0f};
//float fillColor[3] = {1.0f, 0.0f, 0.0f};
//
//// Function to get pixel color at a given coordinate
//void getPixelColor(int x, int y, float color[3])
//{
//    glReadPixels(x, y, 1, 1, GL_RGB, GL_FLOAT, color);
//}
//
//// Function to set pixel color
//void setPixelColor(int x, int y, float color[3])
//{
//    glBegin(GL_POINTS);
//    glColor3fv(color);
//    glVertex2i(x, y);
//    glEnd();
//    glFlush();
//}
//
//// Recursive Boundary Fill Algorithm
//void boundaryFill(int x, int y)
//{
//    float currentColor[3];
//    getPixelColor(x, 500 - y, currentColor);
//
//    // Check if current pixel is not boundary and not already filled
//    if ((currentColor[0] != boundaryColor[0] || currentColor[1] != boundaryColor[1] || currentColor[2] != boundaryColor[2]) &&
//        (currentColor[0] != fillColor[0] || currentColor[1] != fillColor[1] || currentColor[2] != fillColor[2]))
//    {
//        setPixelColor(x, y, fillColor);
//
//        // Recursive calls for 4-connected neighbors
//        boundaryFill(x + 1, y);
//        boundaryFill(x - 1, y);
//        boundaryFill(x, y + 1);
//        boundaryFill(x, y - 1);
//    }
//}
//
//// Function to draw a polygon
//void drawPolygon(int x, int y, int width, int height)
//{
//    glColor3f(1.0, 0.0, 0.0); // Black boundary color
//    glBegin(GL_LINE_LOOP);
//    glVertex2i(x, y);
//    glVertex2i(x + width, y);
//    glVertex2i(x + width, y + height);
//    glVertex2i(x, y + height);
//    glEnd();
//    glFlush();
//}
//
//// Display function
//void display()
//{
//    glClear(GL_COLOR_BUFFER_BIT);
//    drawPolygon(50, 50, 60, 60);
//    boundaryFill(60, 70); // Convert OpenGL coordinates
//}
//
//int lab3_task_4_1_main(int argc, char **argv)
//{
//    glutInit(&argc, argv);
//    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
//    glutInitWindowSize(500, 500); // Here we set the window size
//    glutCreateWindow("Task 4.1");
//    glClearColor(0.0f, 0.0f, 0.05f, 1.0f); // Set background color to black
//    glMatrixMode(GL_PROJECTION);           // Switch to projection matrix
//    glLoadIdentity();                      // Reset the matrix
//    gluOrtho2D(0, 500, 500, 0);            // Set orthographic projection
//    glutDisplayFunc(display);
//    glutMainLoop();
//    return 0;
//}