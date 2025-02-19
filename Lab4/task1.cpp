// #include <GL/glut.h>
// #include <assimp/Importer.hpp>
// #include <assimp/scene.h>
// #include <assimp/postprocess.h>
// #include <vector>
// #include <iostream>

// // Model transformation variables-155, -40, -100,
// float modelX = -155.0f, modelY = -40.0f, modelZ = -100.0f; // Model position
// float modelRotX = -60.0f, modelRotY = 0.0f, modelRotZ = -30.0f; // Model rotation

// // Lighting variables
// GLfloat lightPos[] = { 2.0f, 2.0f, 2.0f, 1.0f }; // Point light position
// GLfloat lightColor[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // Light color (white)

// // Material properties (for Gouraud shading)
// GLfloat matAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
// GLfloat matDiffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
// GLfloat matSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
// GLfloat matShininess[] = { 50.0f };

// // Model data
// std::vector<float> vertices;
// std::vector<float> normals;
// std::vector<unsigned int> indices;

// // Function to load a 3D model using Assimp
// bool loadModel(const std::string& path) {
//     Assimp::Importer importer;
//     const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

//     if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
//         std::cerr << "Error loading model: " << importer.GetErrorString() << std::endl;
//         return false;
//     }

//     // Process the first mesh (for simplicity)
//     aiMesh* mesh = scene->mMeshes[0];

//     // Extract vertices and normals
//     for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
//         // Vertices
//         vertices.push_back(mesh->mVertices[i].x);
//         vertices.push_back(mesh->mVertices[i].y);
//         vertices.push_back(mesh->mVertices[i].z);

//         // Normals
//         normals.push_back(mesh->mNormals[i].x);
//         normals.push_back(mesh->mNormals[i].y);
//         normals.push_back(mesh->mNormals[i].z);
//     }

//     // Extract indices
//     for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
//         aiFace face = mesh->mFaces[i];
//         for (unsigned int j = 0; j < face.mNumIndices; j++) {
//             indices.push_back(face.mIndices[j]);
//         }
//     }

//     return true;
// }

// // Function to render the model
// void renderModel() {
//     glBegin(GL_TRIANGLES);
//     for (unsigned int i = 0; i < indices.size(); i++) {
//         unsigned int index = indices[i];
//         glNormal3f(normals[index * 3], normals[index * 3 + 1], normals[index * 3 + 2]);
//         glVertex3f(vertices[index * 3], vertices[index * 3 + 1], vertices[index * 3 + 2]);
//     }
//     glEnd();
// }

// // Function to render the scene
// void display() {
//     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//     glLoadIdentity();

//     // Set up the camera (fixed camera, move the model instead)
//     gluLookAt(0.0f, 0.0f, 5.0f,  // Camera position
//         0.0f, 0.0f, 0.0f,  // Look at point
//         0.0f, 1.0f, 0.0f); // Up vector

//     // Apply model transformations
//     glTranslatef(modelX, modelY, modelZ); // Move the model
//     glRotatef(modelRotX, 1.0f, 0.0f, 0.0f); // Rotate around X axis
//     glRotatef(modelRotY, 0.0f, 1.0f, 0.0f); // Rotate around Y axis
//     glRotatef(modelRotZ, 0.0f, 0.0f, 1.0f); // Rotate around Z axis

//     // Render the model
//     renderModel();

//     glutSwapBuffers();
// }

// // Function to handle window resizing
// void reshape(int w, int h) {
//     glViewport(0, 0, w, h);
//     glMatrixMode(GL_PROJECTION);
//     glLoadIdentity();
//     gluPerspective(45.0f, (float)w / (float)h, 0.1f, 10000.0f); // Set perspective projection
//     glMatrixMode(GL_MODELVIEW);
// }

// // Function to initialize OpenGL settings
// void init() {
//     glEnable(GL_DEPTH_TEST); // Enable depth testing
//     glEnable(GL_NORMALIZE);  // Normalize normals for lighting
//     //setupLighting();         // Set up lighting

//     // Load the model
//     if (!loadModel("model.stl")) {
//         std::cerr << "Failed to load model" << std::endl;
//         exit(1);
//     }
// }

// int main(int argc, char** argv) {
//     glutInit(&argc, argv);
//     glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
//     glutInitWindowSize(800, 600);
//     glutCreateWindow("Task1 - Loading A 3D model STL/OBJ");

//     init();

//     glutDisplayFunc(display);
//     glutReshapeFunc(reshape);

//     glutMainLoop();
//     return 0;
// }