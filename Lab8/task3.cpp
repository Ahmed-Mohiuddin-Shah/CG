#include <GL/glut.h>
#include <vector>
#include <iostream>
#include <cmath>

const int GRID_SIZE = 10; // Size of the grid (10x10)
const int CELL_SIZE = 50; // Size of each cell in pixels
const int WINDOW_WIDTH = GRID_SIZE * CELL_SIZE;
const int WINDOW_HEIGHT = GRID_SIZE * CELL_SIZE;

struct Object {
    int x, y; // Position in the grid
    int width, height; // Size of the object
    float radius; // Radius for spherical bounding box
    float vx, vy; // Velocity
    bool selected; // Whether the object is selected
};

std::vector<Object> objects; // List of objects in the scene
std::vector<std::vector<std::vector<Object*>>> grid(GRID_SIZE, std::vector<std::vector<Object*>>(GRID_SIZE)); // Uniform grid
int selectedObjectIndex = -1; // Index of the currently selected object

// Function to draw a rectangle
void drawRect(int x, int y, int width, int height) {
    glBegin(GL_QUADS);
    glVertex2i(x, y);
    glVertex2i(x + width, y);
    glVertex2i(x + width, y + height);
    glVertex2i(x, y + height);
    glEnd();
}

// Function to draw a circle
void drawCircle(int x, int y, float radius) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y); // Center of the circle
    for (int i = 0; i <= 360; i++) {
        float angle = i * 3.14159 / 180;
        glVertex2f(x + cos(angle) * radius, y + sin(angle) * radius);
    }
    glEnd();
}

// Function to draw the grid
void drawGrid() {
    glColor3f(0.5, 0.5, 0.5); // Gray color for the grid
    for (int i = 0; i <= GRID_SIZE; ++i) {
        glBegin(GL_LINES);
        glVertex2i(i * CELL_SIZE, 0);
        glVertex2i(i * CELL_SIZE, WINDOW_HEIGHT);
        glVertex2i(0, i * CELL_SIZE);
        glVertex2i(WINDOW_WIDTH, i * CELL_SIZE);
        glEnd();
    }
}

// Function to draw objects
void drawObjects() {
    for (const auto& obj : objects) {
        if (obj.selected) {
            glColor3f(0.0, 1.0, 0.0); // Green color for selected object
        }
        else {
            glColor3f(0.0, 0.0, 1.0); // Blue color for other objects
        }
        drawRect(obj.x, obj.y, obj.width, obj.height);

        // Draw spherical bounding box
        glColor3f(1.0, 0.0, 0.0); // Red color for spherical bounding box
        drawCircle(obj.x + obj.width / 2, obj.y + obj.height / 2, obj.radius);
    }
}

// Function to check for AABB collisions
bool checkAABBCollision(const Object& obj1, const Object& obj2) {
    return (obj1.x < obj2.x + obj2.width &&
        obj1.x + obj1.width > obj2.x &&
        obj1.y < obj2.y + obj2.height &&
        obj1.y + obj1.height > obj2.y);
}

// Function to check for spherical collisions
bool checkSphericalCollision(const Object& obj1, const Object& obj2) {
    float dx = (obj1.x + obj1.width / 2) - (obj2.x + obj2.width / 2);
    float dy = (obj1.y + obj1.height / 2) - (obj2.y + obj2.height / 2);
    float distance = sqrt(dx * dx + dy * dy);
    return distance < (obj1.radius + obj2.radius);
}

// Function to handle collisions and reflections
void handleCollisions() {
    for (size_t i = 0; i < objects.size(); ++i) {
        for (size_t j = i + 1; j < objects.size(); ++j) {
            if (checkAABBCollision(objects[i], objects[j])) {
                // Reflect velocities for AABB collision
                std::swap(objects[i].vx, objects[j].vx);
                std::swap(objects[i].vy, objects[j].vy);
            }
            if (checkSphericalCollision(objects[i], objects[j])) {
                // Reflect velocities for spherical collision
                std::swap(objects[i].vx, objects[j].vx);
                std::swap(objects[i].vy, objects[j].vy);
            }
        }

        // Reflect off walls
        if (objects[i].x < 0 || objects[i].x + objects[i].width > WINDOW_WIDTH) {
            objects[i].vx = -objects[i].vx;
        }
        if (objects[i].y < 0 || objects[i].y + objects[i].height > WINDOW_HEIGHT) {
            objects[i].vy = -objects[i].vy;
        }
    }
}

// Function to assign objects to grid cells
void assignObjectsToGrid() {
    // Clear the grid
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            grid[i][j].clear();
        }
    }

    // Assign objects to grid cells
    for (auto& obj : objects) {
        int gridX = obj.x / CELL_SIZE;
        int gridY = obj.y / CELL_SIZE;
        if (gridX >= 0 && gridX < GRID_SIZE && gridY >= 0 && gridY < GRID_SIZE) {
            grid[gridX][gridY].push_back(&obj);
        }
    }
}

// Function to update object positions
void update(int value) {
    for (auto& obj : objects) {
        obj.x += obj.vx;
        obj.y += obj.vy;
    }

    handleCollisions();
    assignObjectsToGrid();
    glutPostRedisplay();
    glutTimerFunc(16, update, 0); // 60 FPS
}

// Function to handle keyboard input
void keyboard(unsigned char key, int x, int y) {
    if (selectedObjectIndex != -1) {
        Object& selectedObject = objects[selectedObjectIndex];
        switch (key) {
        case 'w': // Move up
            selectedObject.y += 10;
            break;
        case 'a': // Move left
            selectedObject.x -= 10;
            break;
        case 's': // Move down
            selectedObject.y -= 10;
            break;
        case 'd': // Move right
            selectedObject.x += 10;
            break;
        case ' ': // Deselect object
            selectedObject.selected = false;
            selectedObjectIndex = -1;
            break;
        }
        // Reassign objects to grid after movement
        assignObjectsToGrid();
        glutPostRedisplay(); // Request redraw
    }
}

// Function to handle mouse click for object selection
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // Convert screen coordinates to grid coordinates
        int gridX = x / CELL_SIZE;
        int gridY = (WINDOW_HEIGHT - y) / CELL_SIZE;

        // Check if there is an object in the clicked cell
        if (gridX >= 0 && gridX < GRID_SIZE && gridY >= 0 && gridY < GRID_SIZE) {
            for (auto obj : grid[gridX][gridY]) {
                if (x >= obj->x && x <= obj->x + obj->width && (WINDOW_HEIGHT - y) >= obj->y && (WINDOW_HEIGHT - y) <= obj->y + obj->height) {
                    // Deselect previously selected object
                    if (selectedObjectIndex != -1) {
                        objects[selectedObjectIndex].selected = false;
                    }
                    // Select new object
                    obj->selected = true;
                    selectedObjectIndex = std::distance(objects.data(), obj);
                    break;
                }
            }
        }
        glutPostRedisplay(); // Request redraw
    }
}

// Display callback function
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawGrid();
    drawObjects();
    glutSwapBuffers();
}

// Initialize objects and grid
void init() {
    // Add some objects to the scene
    objects.push_back({ 100, 200, 30, 30, 20, 2, 2, false });
    objects.push_back({ 120, 120, 30, 30, 20, -2, 2, false });
    objects.push_back({ 300, 100, 30, 30, 20, 2, -2, false });
    objects.push_back({ 320, 320, 30, 30, 20, -2, -2, false });
	objects.push_back({ 200, 200, 30, 30, 20, 1, 1, false });
	objects.push_back({ 250, 250, 30, 30, 20, -1, 1, false });

    // Assign objects to grid cells
    assignObjectsToGrid();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Task3 - Uniform Grid AABB and Spherical");

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);

    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard); // Register keyboard callback
    glutMouseFunc(mouse); // Register mouse callback
    glutTimerFunc(0, update, 0); // Start the update loop
    glutMainLoop();
    return 0;
}