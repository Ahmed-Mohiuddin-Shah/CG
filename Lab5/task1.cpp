﻿//#include "helpers.h"
//
//int main() {
//   GLFWwindow *window = initWindow(std::string("Task1: Basic Raytrace"));
//
//   std::vector<Sphere> spheres;
//		spheres.push_back(Sphere(Vector3(0.0f, 0.0f, -3.0f), 1.0f)); // Center sphere
//		spheres.push_back(Sphere(Vector3(2.0f, 0.0f, -5.0f), 1.0f)); // Right sphere
//		spheres.push_back(Sphere(Vector3(-2.0f, 0.0f, -5.0f), 1.0f)); // Left sphere
//
//   while (!glfwWindowShouldClose(window)) {
//       renderBasicRaytraceScene(spheres);
//       glfwSwapBuffers(window);
//       glfwPollEvents();
//   }
//
//   glfwDestroyWindow(window);
//   glfwTerminate();
//   return 0;
//}