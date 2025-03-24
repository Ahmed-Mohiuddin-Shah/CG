// #include <GL/glut.h>
// #include <vector>
// #include <iostream>

// const int GRID_SIZE = 10; // Size of the grid (10x10)
// const int CELL_SIZE = 50; // Size of each cell in pixels
// const int WINDOW_WIDTH = GRID_SIZE * CELL_SIZE;
// const int WINDOW_HEIGHT = GRID_SIZE * CELL_SIZE;

// struct Object {
//     int x, y; // Position in the grid
//     int width, height; // Size of the object
//     bool selected; // Whether the object is selected
// };

// std::vector<Object> objects; // List of objects in the scene
// std::vector<std::vector<std::vector<Object*>>> grid(GRID_SIZE, std::vector<std::vector<Object*>>(GRID_SIZE)); // Uniform grid
// int selectedObjectIndex = -1; // Index of the currently selected object

// // Function to draw a rectangle
// void drawRect(int x, int y, int width, int height) {
//     glBegin(GL_QUADS);
//     glVertex2i(x, y);
//     glVertex2i(x + width, y);
//     glVertex2i(x + width, y + height);
//     glVertex2i(x, y + height);
//     glEnd();
// }

// // Function to draw the grid
// void drawGrid() {
//     glColor3f(0.5, 0.5, 0.5); // Gray color for the grid
//     for (int i = 0; i <= GRID_SIZE; ++i) {
//         glBegin(GL_LINES);
//         glVertex2i(i * CELL_SIZE, 0);
//         glVertex2i(i * CELL_SIZE, WINDOW_HEIGHT);
//         glVertex2i(0, i * CELL_SIZE);
//         glVertex2i(WINDOW_WIDTH, i * CELL_SIZE);
//         glEnd();
//     }
// }

// // Function to draw objects
// void drawObjects() {
//     for (const auto& obj : objects) {
//         if (obj.selected) {
//             glColor3f(0.0, 1.0, 0.0); // Green color for selected object
//         }
//         else {
//             glColor3f(0.0, 0.0, 1.0); // Blue color for other objects
//         }
//         drawRect(obj.x, obj.y, obj.width, obj.height);
//     }
// }

// // Function to check for collisions and highlight them
// void checkCollisions() {
//     for (int i = 0; i < GRID_SIZE; ++i) {
//         for (int j = 0; j < GRID_SIZE; ++j) {
//             if (grid[i][j].size() > 1) { // More than one object in the cell
//                 for (auto obj : grid[i][j]) {
//                     glColor3f(1.0, 0.0, 0.0); // Red color for collisions
//                     drawRect(obj->x, obj->y, obj->width, obj->height);
//                 }
//             }
//         }
//     }
// }

// // Function to assign objects to grid cells
// void assignObjectsToGrid() {
//     // Clear the grid
//     for (int i = 0; i < GRID_SIZE; ++i) {
//         for (int j = 0; j < GRID_SIZE; ++j) {
//             grid[i][j].clear();
//         }
//     }

//     // Assign objects to grid cells
//     for (auto& obj : objects) {
//         int gridX = obj.x / CELL_SIZE;
//         int gridY = obj.y / CELL_SIZE;
//         if (gridX >= 0 && gridX < GRID_SIZE && gridY >= 0 && gridY < GRID_SIZE) {
//             grid[gridX][gridY].push_back(&obj);
//         }
//     }
// }

// // Function to handle keyboard input
// void keyboard(unsigned char key, int x, int y) {
//     if (selectedObjectIndex != -1) {
//         Object& selectedObject = objects[selectedObjectIndex];
//         switch (key) {
//         case 'w': // Move up
//             selectedObject.y += 10;
//             break;
//         case 'a': // Move left
//             selectedObject.x -= 10;
//             break;
//         case 's': // Move down
//             selectedObject.y -= 10;
//             break;
//         case 'd': // Move right
//             selectedObject.x += 10;
//             break;
//         case ' ': // Deselect object
//             selectedObject.selected = false;
//             selectedObjectIndex = -1;
//             break;
//         }
//         // Reassign objects to grid after movement
//         assignObjectsToGrid();
//         glutPostRedisplay(); // Request redraw
//     }
// }

// // Function to handle mouse click for object selection
// void mouse(int button, int state, int x, int y) {
//     if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
//         // Convert screen coordinates to grid coordinates
//         int gridX = x / CELL_SIZE;
//         int gridY = (WINDOW_HEIGHT - y) / CELL_SIZE;

//         // Check if there is an object in the clicked cell
//         if (gridX >= 0 && gridX < GRID_SIZE && gridY >= 0 && gridY < GRID_SIZE) {
//             for (auto obj : grid[gridX][gridY]) {
//                 if (x >= obj->x && x <= obj->x + obj->width && (WINDOW_HEIGHT - y) >= obj->y && (WINDOW_HEIGHT - y) <= obj->y + obj->height) {
//                     // Deselect previously selected object
//                     if (selectedObjectIndex != -1) {
//                         objects[selectedObjectIndex].selected = false;
//                     }
//                     // Select new object
//                     obj->selected = true;
//                     selectedObjectIndex = std::distance(objects.data(), obj);
//                     break;
//                 }
//             }
//         }
//         glutPostRedisplay(); // Request redraw
//     }
// }

// // Display callback function
// void display() {
//     glClear(GL_COLOR_BUFFER_BIT);
//     drawGrid();
//     drawObjects();
//     checkCollisions();
//     glutSwapBuffers();
// }

// // Initialize objects and grid
// void init() {
//     // Add some objects to the scene
//     objects.push_back({ 100, 100, 30, 30, false });
//     objects.push_back({ 120, 120, 30, 30, false });
//     objects.push_back({ 300, 300, 30, 30, false });
//     objects.push_back({ 320, 320, 30, 30, false });

//     // Assign objects to grid cells
//     assignObjectsToGrid();
// }

// int main(int argc, char** argv) {
//     glutInit(&argc, argv);
//     glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
//     glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
//     glutCreateWindow("Task2 - Uniform Grid");

//     glMatrixMode(GL_PROJECTION);
//     glLoadIdentity();
//     gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);

//     init();
//     glutDisplayFunc(display);
//     glutKeyboardFunc(keyboard); // Register keyboard callback
//     glutMouseFunc(mouse); // Register mouse callback
//     glutMainLoop();
//     return 0;
// }