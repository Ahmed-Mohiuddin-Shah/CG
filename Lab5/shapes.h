#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>

// Vector3 class for handling 3D vectors
class Vector3 {
public:
   float x, y, z;

   Vector3() : x(0), y(0), z(0) {}
   Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

   Vector3 operator+(const Vector3& v) const { return Vector3(x + v.x, y + v.y, z + v.z); }
   Vector3 operator-(const Vector3& v) const { return Vector3(x - v.x, y - v.y, z - v.z); }
   Vector3 operator-() const { return Vector3(-x, -y, -z); }
   Vector3 operator*(float f) const { return Vector3(x * f, y * f, z * f); }
   Vector3 operator*(const Vector3& v) const { 
       return Vector3(
           y * v.z - z * v.y,
           z * v.x - x * v.z,
           x * v.y - y * v.x
       );
   }
   float dot(const Vector3& v) const { return x * v.x + y * v.y + z * v.z; }
   Vector3 cross(const Vector3& v) const {
       return Vector3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
   }
   Vector3 normalize() const {
       float mag = sqrt(x * x + y * y + z * z);
       return Vector3(x / mag, y / mag, z / mag);
   }
};

// Ray class
class Ray {
public:
   Vector3 origin;
   Vector3 direction;

   Ray(const Vector3& origin, const Vector3& direction) : origin(origin), direction(direction.normalize()) {}
};

// Sphere class
class Sphere {
public:
   Vector3 center;
   float radius;

   Sphere(const Vector3& center, float radius) : center(center), radius(radius) {}

   bool intersect(const Ray& ray, float& t) const {
       Vector3 oc = ray.origin - center;
       float a = ray.direction.dot(ray.direction);
       float b = 2.0f * oc.dot(ray.direction);
       float c = oc.dot(oc) - radius * radius;
       float discriminant = b * b - 4 * a * c;

       if (discriminant < 0) {
           return false;
       }
       else {
           t = (-b - sqrt(discriminant)) / (2.0f * a);
           return true;
       }
   }

   Vector3 getNormal(const Vector3& point) const {
       return (point - center).normalize();
   }
};


// Plane class
class Plane {
public:
   Vector3 normal;
   float distance;

   Plane(const Vector3& normal, float distance) : normal(normal.normalize()), distance(distance) {}

   bool intersect(const Ray& ray, float& t) const {
       float denom = normal.dot(ray.direction);
       if (abs(denom) > 1e-6) { // Avoid division by zero
           Vector3 p0 = normal * distance; // A point on the plane
           t = (p0 - ray.origin).dot(normal) / denom;
           return (t >= 0);
       }
       return false;
   }

   Vector3 getNormal() const {
       return normal;
   }
};

// Light class
class Light {
public:
   Vector3 position;
   Vector3 intensity;

   Light(const Vector3& position, const Vector3& intensity) : position(position), intensity(intensity) {}
};

// Function to handle keyboard input
void processInput(GLFWwindow* window, Light& light) {
    float moveSpeed = 1.0f; // Speed of light movement

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        light.position.y += moveSpeed; // Move light up
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        light.position.y -= moveSpeed; // Move light down
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        light.position.x -= moveSpeed; // Move light left
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        light.position.x += moveSpeed; // Move light right
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        light.position.z -= moveSpeed; // Move light forward
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        light.position.z += moveSpeed; // Move light backward
    }
}
