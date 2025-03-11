//#include <GL/glut.h>
//#include <math.h>
//#include "helpers.h"
//
//Params params;
//int screenWidth = 800;
//int screenHeight = 800;
//bool toggle = false;
//
//void display()
//{
//    glClear(GL_COLOR_BUFFER_BIT);
//	drawViewPort(&params.viewPort, screenWidth, screenHeight);
//
//	for (auto line : params.lines)
//	{
//		if (toggle) {
//			glColor3f((rand() % 255) / 255.0, (rand() % 255) / 255.0, (rand() % 255) / 255.0);
//			glBegin(GL_LINES);
//			glVertex2i(line.x1, line.y1);
//			glVertex2i(line.x2, line.y2);
//			glEnd();
//			continue;
//		}
//
//		drawCohenSutherlandLineClip(line, params.viewPort);
//	}
//	glFlush();
//}
//
//void keyboard(unsigned char key, int x, int y)
//{
//	if (key == 27)
//		exit(0);
//
//	// w key
//	if (key == 119)
//	{
//		toggle = !toggle;
//	}
//
//	// rerender
//	glutPostRedisplay();
//}
//
//
//int main(int argc, char** argv)
//{
//
//    params = getParams(true, false);
//
//    glutInit(&argc, argv);
//    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
//	glutInitWindowSize(screenWidth, screenHeight);
//    glutCreateWindow("Task1 - Cohen Sutherland Line Clipping");
//    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    gluOrtho2D(0.0, 800.0, 0.0, 800.0);
//    glutDisplayFunc(display);
//	glutKeyboardFunc(keyboard);
//    glutMainLoop();
//    return 0;
//}
