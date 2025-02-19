// #include <GL/glut.h>
// #include <assimp/Importer.hpp>
// #include <assimp/scene.h>
// #include <assimp/postprocess.h>
// #include <vector>
// #include <iostream>
// #include <cmath>
// #include <glm/glm.hpp>

// // Model transformation variables-155, -40, -100,
// float modelX = -155.0f, modelY = -40.0f, modelZ = -100.0f; // Model position
// float modelRotX = -60.0f, modelRotY = 0.0f, modelRotZ = -30.0f; // Model rotation

// // Light properties
// glm::vec3 lightPos = glm::vec3(170.0f, 80.0f, 80.0f); // Light position
// glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f); // Light color (white)
// float ambientIntensity = 0.2f; // Ambient intensity
// float diffuseIntensity = 0.8f; // Diffuse intensity
// float specularIntensity = 1.0f; // Specular intensity
// float shininess = 50.0f; // Shininess exponent

// // Material properties (model color is red)
// glm::vec3 matAmbient = glm::vec3(0.7f, 0.0f, 0.0f); // Ambient reflection (red)
// glm::vec3 matDiffuse = glm::vec3(0.8f, 0.0f, 0.0f); // Diffuse reflection (red)
// glm::vec3 matSpecular = glm::vec3(0.5f, 0.5f, 0.5f); // Specular reflection (white)

// // Model data
// std::vector<glm::vec3> vertices;
// std::vector<glm::vec3> normals;
// std::vector<unsigned int> indices;
// std::vector<glm::vec3> vertexColors; // Colors for Gouraud shading

// // Function to load a 3D model using Assimp
// bool loadModel(const std::string& path) {
//    Assimp::Importer importer;
//    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

//    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
//        std::cerr << "Error loading model: " << importer.GetErrorString() << std::endl;
//        return false;
//    }

//    // Process the first mesh (for simplicity)
//    aiMesh* mesh = scene->mMeshes[0];

//    // Extract vertices and normals
//    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
//        // Vertices
//        vertices.push_back(glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));

//        // Normals
//        normals.push_back(glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z));
//    }

//    // Extract indices
//    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
//        aiFace face = mesh->mFaces[i];
//        for (unsigned int j = 0; j < face.mNumIndices; j++) {
//            indices.push_back(face.mIndices[j]);
//        }
//    }

//    return true;
// }

// // Function to compute lighting for a vertex
// glm::vec3 computeLighting(const glm::vec3& vertex, const glm::vec3& normal) {
//    // Ambient component
//    glm::vec3 ambient = matAmbient * ambientIntensity;

//    // Diffuse component
//    glm::vec3 lightDir = glm::normalize(lightPos - vertex);
//    float diff = glm::max(glm::dot(normal, lightDir), 0.0f);
//    glm::vec3 diffuse = matDiffuse * diffuseIntensity * diff;

//    // Specular component
//    glm::vec3 viewDir = glm::normalize(glm::vec3(0.0f, 0.0f, 5.0f) - vertex); // Camera at (0, 0, 5)
//    glm::vec3 reflectDir = glm::reflect(-lightDir, normal);
//    float spec = glm::pow(glm::max(glm::dot(viewDir, reflectDir), 0.0f), shininess);
//    glm::vec3 specular = matSpecular * specularIntensity * spec;

//    // Total lighting
//    return ambient + diffuse + specular;
// }

// // Function to calculate vertex colors for Gouraud shading
// void calculateVertexColors() {
//    vertexColors.clear();
//    for (size_t i = 0; i < vertices.size(); i++) {
//        glm::vec3 color = computeLighting(vertices[i], normals[i]);
//        vertexColors.push_back(color);
//    }
// }

// // Function to render the model with Gouraud shading
// void renderModel() {
//    glBegin(GL_TRIANGLES);
//    for (size_t i = 0; i < indices.size(); i += 3) {
//        // Get vertices and normals for the triangle
//        glm::vec3 v0 = vertices[indices[i]];
//        glm::vec3 v1 = vertices[indices[i + 1]];
//        glm::vec3 v2 = vertices[indices[i + 2]];

//        glm::vec3 n0 = normals[indices[i]];
//        glm::vec3 n1 = normals[indices[i + 1]];
//        glm::vec3 n2 = normals[indices[i + 2]];

//        // Compute lighting for each vertex
//        glm::vec3 color0 = computeLighting(v0, n0);
//        glm::vec3 color1 = computeLighting(v1, n1);
//        glm::vec3 color2 = computeLighting(v2, n2);

//        // Draw the triangle with interpolated colors
//        glColor3f(color0.x, color0.y, color0.z);
//        glVertex3f(v0.x, v0.y, v0.z);

//        glColor3f(color1.x, color1.y, color1.z);
//        glVertex3f(v1.x, v1.y, v1.z);

//        glColor3f(color2.x, color2.y, color2.z);
//        glVertex3f(v2.x, v2.y, v2.z);
//    }
//    glEnd();
// }

// // Function to draw a small sphere for the light
// void drawLight() {
//    glPushMatrix();
//    glTranslatef(lightPos.x, lightPos.y, lightPos.z); // Move to light position
//    glColor3f(1.0f, 1.0f, 1.0f); // Light color (white)
//    glutSolidSphere(10.0f, 30, 30); // Draw a small sphere
//    glPopMatrix();
// }

// void drawInfinitePlane() {
//    const float gridSize = 1000.0f; // Size of the grid (extent in each direction)
//    const float gridStep = 10.0f;  // Distance between grid lines

//    // Enable blending for transparency (optional)
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//    // Set the color of the grid lines (light gray)
//    glColor4f(0.7f, 0.7f, 0.7f, 0.5f); // RGBA (A = alpha for transparency)

//    // Draw the grid
//    glBegin(GL_LINES);
//    for (float i = -gridSize; i <= gridSize; i += gridStep) {
//        // Horizontal lines
//        glVertex3f(-gridSize, i, 0.0f);
//        glVertex3f(gridSize, i, 0.0f);

//        // Vertical lines
//        glVertex3f(i, -gridSize, 0.0f);
//        glVertex3f(i, gridSize, 0.0f);
//    }
//    glEnd();

//    // Disable blending (if enabled)
//    glDisable(GL_BLEND);
// }

// // Function to handle keyboard input (move the light)
// void keyboard(unsigned char key, int x, int y) {
//    float moveAmount = 10.0f; // Light movement speed

//    switch (key) {
//    case 'w': lightPos.x += moveAmount; break; // Move light forward
//    case 's': lightPos.x -= moveAmount; break; // Move light backward
//    case 'a': lightPos.y -= moveAmount; break; // Move light left
//    case 'd': lightPos.y += moveAmount; break; // Move light right
//    case 'q': lightPos.z += moveAmount; break; // Move light up
//    case 'e': lightPos.z -= moveAmount; break; // Move light down
//    }

//    std::cout << lightPos.x << ", " << lightPos.y << ", " << lightPos.z << ", " << std::endl;

//    // Recalculate vertex colors for Gouraud shading
//    calculateVertexColors();

//    glutPostRedisplay(); // Redraw the scene
// }

// // Function to handle mouse motion (rotate model)
// void mouseMotion(int x, int y) {
//    static int lastX = x, lastY = y;
//    int deltaX = x - lastX;
//    int deltaY = y - lastY;

//    modelRotY += deltaX * 0.5f; // Rotate around Y axis
//    modelRotX += deltaY * 0.5f; // Rotate around X axis

//    lastX = x;
//    lastY = y;

//    glutPostRedisplay(); // Redraw the scene
// }

// // Function to render the scene
// void display() {
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glLoadIdentity();

//    // Set up the camera (fixed camera, move the model instead)
//    gluLookAt(0.0f, 0.0f, 5.0f,  // Camera position
//        0.0f, 0.0f, 0.0f,  // Look at point
//        0.0f, 1.0f, 0.0f); // Up vector

//    // Apply model transformations
//    glTranslatef(modelX, modelY, modelZ); // Move the model
//    glRotatef(modelRotX, 1.0f, 0.0f, 0.0f); // Rotate around X axis
//    glRotatef(modelRotY, 0.0f, 1.0f, 0.0f); // Rotate around Y axis
//    glRotatef(modelRotZ, 0.0f, 0.0f, 1.0f); // Rotate around Z axis
   
//    drawInfinitePlane();

//    // Render the model
//    renderModel();

//    // Draw the light
//    drawLight();

//    glutSwapBuffers();
// }

// // Function to initialize OpenGL settings
// void init() {
//    glEnable(GL_DEPTH_TEST); // Enable depth testing

//    // Load the model
//    if (!loadModel("model.stl")) {
//        std::cerr << "Failed to load model" << std::endl;
//        exit(1);
//    }

//    // Calculate vertex colors for Gouraud shading
//    calculateVertexColors();
// }

// // Function to handle window resizing
// void reshape(int w, int h) {
//    glViewport(0, 0, w, h);
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    gluPerspective(60.0f, (float)w / (float)h, 0.1f, 10000.0f); // Set perspective projection
//    glMatrixMode(GL_MODELVIEW);
// }

// int main(int argc, char** argv) {
//    glutInit(&argc, argv);
//    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
//    glutInitWindowSize(800, 600);
//    glutCreateWindow("Task2 - Gouraud Shading");

//    init();

//    glutDisplayFunc(display);
//    glutReshapeFunc(reshape);
//    glutKeyboardFunc(keyboard);
//    glutMotionFunc(mouseMotion);

//    glutMainLoop();
//    return 0;
// }