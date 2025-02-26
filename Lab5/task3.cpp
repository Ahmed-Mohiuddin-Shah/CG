// #include "helpers.h"

// int main() {
//    GLFWwindow *window = initWindow(std::string("Task3: Computing Ray-Object Intersections"));

//    // Define spheres in the scene
//    std::vector<Sphere> spheres;
//    spheres.push_back(Sphere(Vector3(0.0f, 0.0f, -3.0f), 1.0f)); // Center sphere
//    spheres.push_back(Sphere(Vector3(2.0f, 0.0f, -5.0f), 1.0f)); // Right sphere
//    spheres.push_back(Sphere(Vector3(-2.0f, 0.0f, -5.0f), 1.0f)); // Left sphere

//    // Checking intersection functions
//    for (int i = 0; i < spheres.size(); i++) {
//        float t;
//        Ray ray(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f));
//        if (spheres[i].intersect(ray, t)) {
//            std::cout << "Sphere " << i << " intersected at t = " << t << std::endl;
//        }
//    }

//    while (!glfwWindowShouldClose(window)) {
//        renderRaycastingScene(spheres);
//        glfwSwapBuffers(window);
//        glfwPollEvents();
//    }

//    glfwDestroyWindow(window);
//    glfwTerminate();
//    return 0;
// }