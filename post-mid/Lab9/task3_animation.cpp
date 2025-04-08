#include "./helpers.h"
#include "zynmodel.h"

ZModel model;

glm::vec3 carTranslation(0.0f, 0.0f, 0.0f);
float carRotationAngle = 90.0f;
glm::vec3 carRotationAxis(1.0f, 0.0f, 0.0f);

void init()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);

    model.loadModel("plane", "./");
    for (int i = 1; i <= 14; i++)
    {
        model.loadModelTexture(("./anim/" + std::to_string(i) + ".png").c_str());
    }
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

    // model.drawModel(0);
    static int frame = 1;
    static int lastTime = glutGet(GLUT_ELAPSED_TIME);
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    int elapsedTime = currentTime - lastTime;

    if (elapsedTime > 1000 / 15)
    {                             // 30 FPS
        frame = (frame % 14) + 1; // Cycle through frames 1 to 14
        lastTime = currentTime;
    }

    model.drawModel(frame);

    glutSwapBuffers();
}

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
}

// Translate the model using the keyboard WASDQE
void keyboardFunction(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27: // ESC key
        exit(0);
        break;
    case 'w':
        carTranslation.y += 0.2f;
        break;
    case 's':
        carTranslation.y -= 0.2f;
        break;
    case 'a':
        carTranslation.x -= 0.2f;
        break;
    case 'd':
        carTranslation.x += 0.2f;
        break;
    case 'q':
        carRotationAngle += 5.0f;
        break;
    case 'e':
        carRotationAngle -= 5.0f;
        break;
    case 'r':
        carRotationAxis = glm::vec3(1.0f, 0.0f, 0.0f);
        break;
    case 't':
        carRotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
        break;
    case 'y':
        carRotationAxis = glm::vec3(0.0f, 0.0f, 1.0f);
        break;
    default:
        break;
    }
    // Update the model with the new translation and rotation
    model.translateModel(carTranslation);
    model.rotateModel(carRotationAngle, carRotationAxis);
    // reset the car translation and rotation
    carTranslation = glm::vec3(0.0f, 0.0f, 0.0f);
    carRotationAngle = 0.0f;
    glutPostRedisplay();
}

// Rotate the model using the mouse
void mouseFunction(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        // Rotate the model based on mouse position
        float angle = (x - 400) / 400.0f * 180.0f; // Normalize to -180 to 180 degrees
        carRotationAngle += angle;
    }

    // Update the model with the new translation and rotation
    model.translateModel(carTranslation);
    model.rotateModel(carRotationAngle, carRotationAxis);
    // reset the car translation and rotation
    carTranslation = glm::vec3(0.0f, 0.0f, 0.0f);
    carRotationAngle = 0.0f;
    glutPostRedisplay();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Task 3 - Animation using textures");

    init();

    // carRotationAngle = 90.0f;
    // carRotationAxis = glm::vec3(1.0f, 1.0f, 0.0f);
    // // Update the model with the new translation and rotation
    // model.translateModel(carTranslation);
    // model.rotateModel(carRotationAngle, carRotationAxis);
    // model.scaleModel(glm::vec3(0.5f, 0.5f, 0.5f));

    // carRotationAngle = 45.0f;
    // carRotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
    // model.translateModel(carTranslation);
    // model.rotateModel(carRotationAngle, carRotationAxis);

    carTranslation = glm::vec3(0.0f, 0.0f, 0.0f);
    carRotationAngle = 0.0f;

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboardFunction);
    glutMouseFunc(mouseFunction);
    glutIdleFunc(display);
    glutMainLoop();
    return 0;
}