#include "./helpers.h"

// Texture variables
GLuint textureID;
int texWidth, texHeight, texChannels;

void init()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);

    // Load texture
    loadTexture("cat.png", textureID, texWidth, texHeight, texChannels);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Set up orthographic projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);

    // Disable polygon offset
    glDisable(GL_POLYGON_OFFSET_FILL);
    glLineWidth(3.0f);

    // Draw outlines of the triangles
    glDisable(GL_TEXTURE_2D);    // Disable texture for outlines
    glColor3f(0.0f, 0.0f, 0.0f); // Set outline color to red
    glBegin(GL_LINE_LOOP);

    // Outline for the first triangle
    glVertex2f(-0.5f, -0.5f);
    glVertex2f(0.5f, -0.5f);
    glVertex2f(-0.5f, 0.5f);

    // Outline for the second triangle
    glVertex2f(0.5f, -0.5f);
    glVertex2f(0.5f, 0.5f);
    glVertex2f(-0.5f, 0.5f);

    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);

    // Draw textured quad using two triangles
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glBegin(GL_TRIANGLES);

    // First triangle
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(-0.5f, -0.5f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(0.5f, -0.5f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(-0.5f, 0.5f);

    // Second triangle
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(0.5f, -0.5f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(0.5f, 0.5f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(-0.5f, 0.5f);

    glEnd();

    glutSwapBuffers();
}

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Task 1: Dsplaying Texture");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glutMainLoop();
    return 0;
}