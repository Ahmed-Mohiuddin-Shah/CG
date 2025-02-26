#include "shapes.h"
#include "helpers.h"

int main() {
  GLFWwindow *window = initWindow(std::string("Task4: Lighting Models"));

  // Define spheres in the scene
  std::vector<Sphere> spheres;
  spheres.push_back(Sphere(Vector3(0.0f, 0.0f, -3.0f), 1.0f)); // Center sphere
  spheres.push_back(Sphere(Vector3(2.0f, 0.0f, -5.0f), 1.0f)); // Right sphere
  spheres.push_back(Sphere(Vector3(-2.0f, 0.0f, -5.0f), 1.0f)); // Left sphere

  // Define planes in the scene
  std::vector<Plane> planes;
  planes.push_back(Plane(Vector3(0.0f, 1.0f, 0.0f), -1.0f)); // Ground plane

  // Define a light source
  Light light(Vector3(2.0f, 5.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f));

  while (!glfwWindowShouldClose(window)) {
      renderPhongRayCastScene(spheres, planes, light);
      glfwSwapBuffers(window);
      glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
