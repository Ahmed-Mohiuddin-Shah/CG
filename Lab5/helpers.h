#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>
#include "shapes.h"

// Window dimensions
const int WIDTH = 800, HEIGHT = 600;

GLFWwindow* initWindow(std::string task) {
    if (!glfwInit()) {
       std::cerr << "Failed to initialize GLFW\n";
       return NULL;
   }

   GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, task.c_str(), NULL, NULL);
   if (!window) {
       glfwTerminate();
       std::cerr << "Failed to create GLFW window\n";
       return NULL;
   }

   glfwMakeContextCurrent(window);
   glewExperimental = GL_TRUE;
   if (glewInit() != GLEW_OK) {
       std::cerr << "Failed to initialize GLEW\n";
       return NULL;
   }

   glViewport(0, 0, WIDTH, HEIGHT);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   return window;
}

// Function to render the scene
void renderBasicRaytraceScene(const std::vector<Sphere>& spheres) {
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glBegin(GL_POINTS);

   for (int y = 0; y < HEIGHT; ++y) {
       for (int x = 0; x < WIDTH; ++x) {
           float u = (float)x / (float)WIDTH;
           float v = (float)y / (float)HEIGHT;
           Ray ray(Vector3(0, 0, 0), Vector3(u - 0.5f, v - 0.5f, -1.0f));

           float t;
           bool hit = false;
           for (const auto& sphere : spheres) {
               if (sphere.intersect(ray, t)) {
                   hit = true;
                   break;
               }
           }

           if (hit) {
               glColor3f(1.0f, 0.0f, 0.0f); // Red color for hit
           }
           else {
               glColor3f(0.0f, 0.0f, 0.0f); // Black color for no hit
           }
           glVertex2f(u, v);
       }
   }

   glEnd();
}

// Function to render the scene using ray casting
void renderRaycastingScene(const std::vector<Sphere>& spheres) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBegin(GL_POINTS);

    // Virtual camera position
    Vector3 cameraPosition(0.0f, 0.0f, 0.0f);

    // Loop through each pixel on the screen
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            // Convert pixel coordinates to normalized device coordinates (-1 to 1)
            float ndcX = (2.0f * x) / WIDTH - 1.0f;
            float ndcY = 1.0f - (2.0f * y) / HEIGHT;

            // Create a ray from the camera through the pixel
            Vector3 rayDirection(ndcX, ndcY, -1.0f); // Assume the image plane is at z = -1
            Ray ray(cameraPosition, rayDirection);

            // Check for intersections with spheres
            float t;
            bool hit = false;
            for (const auto& sphere : spheres) {
                if (sphere.intersect(ray, t)) {
                    hit = true;
                    break;
                }
            }

            // Set pixel color based on intersection
            if (hit) {
                glColor3f(1.0f, 0.0f, 0.0f); // Red color for hit
            }
            else {
                glColor3f(0.0f, 0.0f, 0.0f); // Black color for no hit
            }
            glVertex2f((float)x / WIDTH, (float)y / HEIGHT);
        }
    }

    glEnd();
}


// Function to compute Phong shading
Vector3 phongShading(const Vector3& point, const Vector3& normal, const Vector3& viewDir, const Light& light, const Vector3& kd, const Vector3& ks, float shininess) {
   // Light direction
   Vector3 lightDir = (light.position - point).normalize();

   // Diffuse component
   float diff = std::max(normal.dot(lightDir), 0.0f);
   Vector3 diffuse = light.intensity * kd * diff;

   // Specular component
   Vector3 reflectDir = (-lightDir + normal * 2.0f * normal.dot(lightDir)).normalize();
   float spec = std::pow(std::max(reflectDir.dot(viewDir), 0.0f), shininess);
   Vector3 specular = light.intensity * ks * spec;

   // Ambient component (constant)
   Vector3 ambient = light.intensity * Vector3(0.1f, 0.1f, 0.1f);

   // Combine components
   return ambient + diffuse + specular;
}

// Function to render the scene using ray casting and Phong shading
void renderPhongRayCastScene(const std::vector<Sphere>& spheres, const std::vector<Plane>& planes, const Light& light) {
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glBegin(GL_POINTS);

   // Virtual camera position
   Vector3 cameraPosition(0.0f, 0.0f, 0.0f);

   // Loop through each pixel on the screen
   for (int y = 0; y < HEIGHT; ++y) {
       for (int x = 0; x < WIDTH; ++x) {
           // Convert pixel coordinates to normalized device coordinates (-1 to 1)
           float ndcX = (2.0f * x) / WIDTH - 1.0f;
           float ndcY = 1.0f - (2.0f * y) / HEIGHT;

           // Create a ray from the camera through the pixel
           Vector3 rayDirection(ndcX, ndcY, -1.0f); // Assume the image plane is at z = -1
           Ray ray(cameraPosition, rayDirection);

           // Variables to store intersection details
           float t;
           Vector3 color(0.0f, 0.0f, 0.0f); // Default color (black)
           bool hit = false;


           // Check for intersections with planes
           for (const auto& plane : planes) {
               if (plane.intersect(ray, t)) {
                   Vector3 point = ray.origin + ray.direction * t;
                   Vector3 normal = plane.getNormal();
                   Vector3 viewDir = (cameraPosition - point).normalize();

                   // Apply Phong shading
                   color = phongShading(point, normal, viewDir, light, Vector3(0.2f, 0.8f, 0.2f), Vector3(1.0f, 1.0f, 1.0f), 32.0f);
                   hit = true;
                   break;
               }
           }
           // Check for intersections with spheres
           for (const auto& sphere : spheres) {
               if (sphere.intersect(ray, t)) {
                   Vector3 point = ray.origin + ray.direction * t;
                   Vector3 normal = sphere.getNormal(point);
                   Vector3 viewDir = (cameraPosition - point).normalize();

                   // Apply Phong shading
                   color = phongShading(point, normal, viewDir, light, Vector3(0.8f, 0.2f, 0.2f), Vector3(1.0f, 1.0f, 1.0f), 32.0f);
                   hit = true;
                   break;
               }
           }

           // Set pixel color
           glColor3f(color.x, color.y, color.z);
           glVertex2f((float)x / WIDTH, (float)y / HEIGHT);
       }
   }

   glEnd();
}