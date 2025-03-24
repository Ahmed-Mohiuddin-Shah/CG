// #include <GL/glut.h>
// #include <vector>
// #include <cmath>
// #include <iostream>

// // Struct for a 2D point
// struct Point {
//    float x, y;
// };

// // Struct for Axis-Aligned Bounding Box (AABB)
// struct AABB {
//    Point min, max;

//    bool intersects(const AABB& other) const {
//        return !(max.x < other.min.x || min.x > other.max.x ||
//            max.y < other.min.y || min.y > other.max.y);
//    }
// };

// // Struct for Spherical Bounding Box
// struct BoundingCircle {
//    Point center;
//    float radius;

//    bool intersects(const BoundingCircle& other) const {
//        float dx = center.x - other.center.x;
//        float dy = center.y - other.center.y;
//        float distanceSquared = dx * dx + dy * dy;
//        float radiusSum = radius + other.radius;
//        return distanceSquared <= radiusSum * radiusSum;
//    }
// };

// // Struct for Shapes
// struct Shape {
//    std::vector<Point> vertices;
//    AABB aabb;
//    BoundingCircle circle;
//    float r, g, b;

//    void updateBoundingVolumes() {
//        if (vertices.empty()) return;

//        float minX = vertices[0].x, maxX = vertices[0].x;
//        float minY = vertices[0].y, maxY = vertices[0].y;
//        for (const auto& v : vertices) {
//            if (v.x < minX) minX = v.x;
//            if (v.x > maxX) maxX = v.x;
//            if (v.y < minY) minY = v.y;
//            if (v.y > maxY) maxY = v.y;
//        }
//        aabb = { {minX, minY}, {maxX, maxY} };

//        // Calculate bounding circle
//        circle.center = { (minX + maxX) / 2, (minY + maxY) / 2 };
//        circle.radius = std::sqrt((maxX - minX) * (maxX - minX) + (maxY - minY) * (maxY - minY)) / 2;
//    }
// };

// std::vector<Shape> shapes;
// int selectedShapeIndex = 0;

// // Draw a shape
// void drawShape(const Shape& shape) {
//    glColor3f(shape.r, shape.g, shape.b);
//    glBegin(GL_POLYGON);
//    for (const auto& v : shape.vertices)
//        glVertex2f(v.x, v.y);
//    glEnd();
// }

// // Draw AABB
// void drawAABB(const AABB& aabb, bool collided = false) {
   
//    glColor3f(0, 1, 0);
//    if (collided) {
//        glColor3f(1, 0, 0);
//    }

//    glBegin(GL_LINE_LOOP);
//    glVertex2f(aabb.min.x, aabb.min.y);
//    glVertex2f(aabb.max.x, aabb.min.y);
//    glVertex2f(aabb.max.x, aabb.max.y);
//    glVertex2f(aabb.min.x, aabb.max.y);
//    glEnd();
// }

// // Draw Bounding Circle
// void drawBoundingCircle(const BoundingCircle& circle, bool collided = false) {

//    glColor3f(0, 1, 0);
//    if (collided) {
//        glColor3f(1, 0, 0);
//    }

//    glBegin(GL_LINE_LOOP);
//    for (int i = 0; i < 360; i += 10) {
//        float theta = i * 3.14159f / 180;
//        glVertex2f(circle.center.x + cos(theta) * circle.radius, circle.center.y + sin(theta) * circle.radius);
//    }
//    glEnd();
// }

// // Render function
// void display() {
//    glClear(GL_COLOR_BUFFER_BIT);

//    for (size_t i = 0; i < shapes.size(); i++) {
//        drawShape(shapes[i]);

//        for (size_t j = 0; j < shapes.size(); j++) {
//            if (i != j) {
//                if (shapes[i].aabb.intersects(shapes[j].aabb)) {
//                    drawAABB(shapes[j].aabb, true);
//                }
//                else {
// 					drawAABB(shapes[j].aabb);
//                }
// 				if (shapes[i].circle.intersects(shapes[j].circle)) {
// 					drawBoundingCircle(shapes[j].circle, true);
// 				}
//                else {
//                    drawBoundingCircle(shapes[j].circle);
//                }
//            }
//        }
//    }

//    glutSwapBuffers();
// }

// // Keyboard input
// void keyboard(unsigned char key, int x, int y) {
//    switch (key) {
//    case 27: exit(0); break;
//    case ' ': // Switch shape selection
//        selectedShapeIndex = (selectedShapeIndex + 1) % shapes.size();
//        break;
//    case 'w':
//        for (auto& v : shapes[selectedShapeIndex].vertices) v.y += 0.05f;
//        break;
//    case 's':
//        for (auto& v : shapes[selectedShapeIndex].vertices) v.y -= 0.05f;
//        break;
//    case 'a':
//        for (auto& v : shapes[selectedShapeIndex].vertices) v.x -= 0.05f;
//        break;
//    case 'd':
//        for (auto& v : shapes[selectedShapeIndex].vertices) v.x += 0.05f;
//        break;
//    }
//    shapes[selectedShapeIndex].updateBoundingVolumes();
//    glutPostRedisplay();
// }

// void initShapes() {
//    Shape triangle = { {{-0.1f, -0.1f}, {0.1f, -0.1f}, {0.0f, 0.1f}}, {}, {}, 1.0f, 0.6f, 0.0f };
//    triangle.updateBoundingVolumes();
//    shapes.push_back(triangle);

// 	Shape hexagon = { {{-0.1f, -0.2f}, {0.1f, -0.2f}, {0.2f, 0.0f}, {0.1f, 0.2f}, {-0.1f, 0.2f}, {-0.2f, 0.0f}}, {}, {}, 1.0f, 0.6f, 0.0f };
// 	hexagon.updateBoundingVolumes();
// 	shapes.push_back(hexagon);
// }

// int main(int argc, char** argv) {
//    glutInit(&argc, argv);
//    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
//    glutInitWindowSize(500, 500);
//    glutCreateWindow("Task1 - Bounding Volumes");

//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    gluOrtho2D(-1, 1, -1, 1);

//    initShapes();

//    glutDisplayFunc(display);
//    glutKeyboardFunc(keyboard);
//    glutMainLoop();
//    return 0;
// }
