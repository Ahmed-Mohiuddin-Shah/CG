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
//	glClear(GL_COLOR_BUFFER_BIT);
//	drawViewPort(&params.viewPort, screenWidth, screenHeight);
//
//	for (auto polygon : params.polygons)
//	{
//		glColor3f(1.0, 0.0, 0.0);
//		if (toggle) {
//			for (int i = 0; i < polygon.points.size(); i++) {
//				glBegin(GL_LINES);
//				glVertex2i(polygon.points[i].x, polygon.points[i].y);
//				glVertex2i(polygon.points[(i + 1) % polygon.points.size()].x, polygon.points[(i + 1) % polygon.points.size()].y);
//				glEnd();
//				glRasterPos2i(polygon.points[i].x + 10, polygon.points[i].y - 20);
//				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'A' + i);
//			}
//			continue;
//		}
//		drawSutherHodgmanPolygonClip(&polygon, params.viewPort);
//	}
//	glFlush();
//}
//
//void keyboard(unsigned char key, int x, int y)
//{
//	if (key == 27)
//		exit(0);
//	// q key
//	if (key == 113)
//	{
//
//	}
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
//	params = getParams(false, true);
//
//	glutInit(&argc, argv);
//	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
//	glutInitWindowSize(screenWidth, screenHeight);
//	glutCreateWindow("Task2 - Sutherland Hodgman Polygon Clipping");
//	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//	gluOrtho2D(0.0, 800.0, 0.0, 800.0);
//	glutDisplayFunc(display);
//	glutKeyboardFunc(keyboard);
//	glutMainLoop();
//	return 0;
//}
