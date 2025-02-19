//#include <GL/freeglut.h>
//
//void drawEllipsePoints(int x, int y, int xc, int yc)
//{
//    // We used the 4 point symmetry of the ellipse to save on number of calculations
//
//    glBegin(GL_POINTS);
//    glVertex2i(x + xc, y + yc);
//    glVertex2i(-x + xc, y + yc);
//    glVertex2i(x + xc, -y + yc);
//    glVertex2i(-x + xc, -y + yc);
//    glEnd();
//}
//void drawEllipse(int xc, int yc, int major, int minor, bool show_diff_color = false)
//{
//    float dx, dy, d1, d2, x, y;
//    x = 0;
//    y = minor;
//
//    // Initial decision parameter of region 1
//    d1 = (minor * minor) - (major * major * minor) +
//         (0.25 * major * major);
//    dx = 2 * minor * minor * x;
//    dy = 2 * major * major * y;
//
//    if (show_diff_color)
//    {
//        glColor3f(0.0f, 1.0f, 1.0f);
//    }
//
//    // For region 1
//    while (dx < dy)
//    {
//
//        // Print points based on 4-way symmetry
//        drawEllipsePoints(x, y, xc, yc);
//
//        // Checking and updating value of
//        // decision parameter based on algorithm
//        if (d1 < 0)
//        {
//            x++;
//            dx = dx + (2 * minor * minor);
//            d1 = d1 + dx + (minor * minor);
//        }
//        else
//        {
//            x++;
//            y--;
//            dx = dx + (2 * minor * minor);
//            dy = dy - (2 * major * major);
//            d1 = d1 + dx - dy + (minor * minor);
//        }
//    }
//
//    if (show_diff_color)
//    {
//        glColor3f(1.0f, 1.0f, 0.0f);
//    }
//
//    // Decision parameter of region 2
//    d2 = ((minor * minor) * ((x + 0.5) * (x + 0.5))) +
//         ((major * major) * ((y - 1) * (y - 1))) -
//         (major * major * minor * minor);
//
//    // Plotting points of region 2
//    while (y >= 0)
//    {
//
//        // Print points based on 4-way symmetry
//        drawEllipsePoints(x, y, xc, yc);
//
//        // Checking and updating parameter
//        // value based on algorithm
//        if (d2 > 0)
//        {
//            y--;
//            dy = dy - (2 * major * major);
//            d2 = d2 + (major * major) - dy;
//        }
//        else
//        {
//            y--;
//            x++;
//            dx = dx + (2 * minor * minor);
//            dy = dy - (2 * major * major);
//            d2 = d2 + dx - dy + (major * major);
//        }
//    }
//    glFlush();
//}
//
//void display()
//{
//    glClear(GL_COLOR_BUFFER_BIT);
//
//    // Drawing Ellipses
//    glColor3f(1.0f, 0.0f, 0.0f);
//    drawEllipse(100, 200, 100, 70);
//    drawEllipse(300, 400, 90, 40, true);
//}
//
//int lab3_task_2_main(int argc, char **argv)
//{
//    glutInit(&argc, argv);
//    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
//    glutInitWindowSize(500, 500); // Here we set the window size
//    glutCreateWindow("Task 2");
//    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black
//    glMatrixMode(GL_PROJECTION);          // Switch to projection matrix
//    glLoadIdentity();                     // Reset the matrix
//    gluOrtho2D(0, 500, 500, 0);           // Set orthographic projection
//    glutDisplayFunc(display);
//    glutMainLoop();
//    return 0;
//}