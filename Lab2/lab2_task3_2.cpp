//#include <GL/freeglut.h>
//#include <math.h>
//#include <iostream>
//#include <chrono>
//
//// This function is used to calculate the execution time of any function
//template <typename Func, typename... Args>
//double calculateExecutionTime(Func func, Args... args)
//{
//    auto start = std::chrono::high_resolution_clock::now();
//    func(args...);
//    auto end = std::chrono::high_resolution_clock::now();
//    std::chrono::duration<double, std::milli> duration = end - start;
//    return duration.count(); // Return time in milliseconds
//}
//
//// DDA Algorithm for Line Drawing
//void drawLineDDA(float x1, float y1, float x2, float y2)
//{
//    float dx = x2 - x1;
//    float dy = y2 - y1;
//    float steps = (fabs(dx) > fabs(dy)) ? fabs(dx) : fabs(dy);
//    float xIncrement = dx / steps;
//    float yIncrement = dy / steps;
//    float x = x1, y = y1;
//    glBegin(GL_POINTS);
//    for (int i = 0; i <= steps; i++)
//    {
//        glVertex2i(round(x), round(y));
//        x += xIncrement;
//        y += yIncrement;
//    }
//    glEnd();
//    glFlush();
//}
//
//// Bresenham’s Line Algorithm
//void drawLineBL(int x1, int y1, int x2, int y2)
//{
//    int dx = abs(x2 - x1), dy = abs(y2 - y1);
//    int sx = (x1 < x2) ? 1 : -1;
//    int sy = (y1 < y2) ? 1 : -1;
//    int err = dx - dy;
//    glBegin(GL_POINTS);
//    while (x1 != x2 || y1 != y2)
//    {
//        glVertex2i(x1, y1);
//        int e2 = 2 * err;
//        if (e2 > -dy)
//        {
//            err -= dy;
//            x1 += sx;
//        }
//        if (e2 < dx)
//        {
//            err += dx;
//            y1 += sy;
//        }
//    }
//    glEnd();
//    glFlush();
//}
//
//// Function to draw a triangle with DDA for one side and Bresenham for the other two
//void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3)
//{
//    glColor3f(1, 0, 0); // Red - DDA
//    int dda_time = calculateExecutionTime(drawLineDDA, x1, y1, x2, y2);
//    std::cout << "DDA Time: " << dda_time << "ms\n";
//
//    glColor3f(0, 1, 0); // Green - Bresenham
//    int bl_time = calculateExecutionTime(drawLineBL, x2, y2, x3, y3);
//    std::cout << "BL Time 1: " << bl_time << "ms\n";
//    drawLineBL(x2, y2, x3, y3);
//
//    glColor3f(0, 0, 1); // Blue - Bresenham
//    bl_time = calculateExecutionTime(drawLineBL, x3, y3, x1, y1);
//    std::cout << "BL Time 2: " << bl_time << "ms\n";
//}
//
//void display()
//{
//    glClear(GL_COLOR_BUFFER_BIT);
//    drawTriangle(200, 300, 100, 100, 300, 100);
//}
//
//int lab2_task3_2_main(int argc, char **argv)
//{
//    glutInit(&argc, argv);
//    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
//    glutInitWindowSize(700, 700); // Here we set the window size
//    glutCreateWindow("Task 3.2");
//    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black
//    glMatrixMode(GL_PROJECTION);          // Switch to projection matrix
//    glLoadIdentity();                     // Reset the matrix
//    gluOrtho2D(0, 700, 700, 0);           // Set orthographic projection
//    glutDisplayFunc(display);
//    glutMainLoop();
//    return 0;
//}