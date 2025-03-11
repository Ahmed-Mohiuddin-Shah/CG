#define GLM_ENABLE_EXPERIMENTAL

#include <cmath>
#include <vector>
#include <iomanip>
#include <iostream>
#include <GL/glut.h>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/postprocess.h>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>


// Field of View Stuff
float FOV = 45.0f;

float zNear = 30.0f, zFar = 100.0f;

// Model transformation variables
float modelX = 0.0f, modelY = 0.0f, modelZ = 0.0f; // Model position
float modelRotX = 90.0f, modelRotY = -180.0f, modelRotZ = 40.0f; // Model rotation

// Light properties
glm::vec3 lightPos = glm::vec3(70.0f, 80.0f, 80.0f); // Light position
glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f); // Light color (white)
float ambientIntensity = 0.2f; // Ambient intensity
float diffuseIntensity = 0.8f; // Diffuse intensity
float specularIntensity = 1.0f; // Specular intensity
float shininess = 50.0f; // Shininess exponent

// Material properties (model color is red)
glm::vec3 matAmbient = glm::vec3(0.7f, 0.0f, 0.0f); // Ambient reflection (red)
glm::vec3 matDiffuse = glm::vec3(0.8f, 0.0f, 0.0f); // Diffuse reflection (red)
glm::vec3 matSpecular = glm::vec3(0.5f, 0.5f, 0.5f); // Specular reflection (white)

// Model data
std::vector<glm::vec3> vertices;
std::vector<glm::vec3> normals;
std::vector<unsigned int> indices;
std::vector<glm::vec3> vertexColors; // Colors for Gouraud shading

// Function to load a 3D model using Assimp
bool loadModel(const std::string& path) {
   Assimp::Importer importer;
   const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

   if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
       std::cerr << "Error loading model: " << importer.GetErrorString() << std::endl;
       return false;
   }

   // Process the first mesh (for simplicity)
   aiMesh* mesh = scene->mMeshes[0];

   // Extract vertices and normals
   for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
       // Vertices
       vertices.push_back(glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));

       // Normals
       normals.push_back(glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z));
   }

   // Extract indices
   for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
       aiFace face = mesh->mFaces[i];
       for (unsigned int j = 0; j < face.mNumIndices; j++) {
           indices.push_back(face.mIndices[j]);
       }
   }

   return true;
}

// Function to compute lighting for a vertex
glm::vec3 computeLighting(const glm::vec3& vertex, const glm::vec3& normal) {
   // Ambient component
   glm::vec3 ambient = matAmbient * ambientIntensity;

   // Diffuse component
   glm::vec3 lightDir = glm::normalize(lightPos - vertex);
   float diff = glm::max(glm::dot(normal, lightDir), 0.0f);
   glm::vec3 diffuse = matDiffuse * diffuseIntensity * diff;

   // Specular component
   glm::vec3 viewDir = glm::normalize(glm::vec3(0.0f, 0.0f, 5.0f) - vertex); // Camera at (0, 0, 5)
   glm::vec3 reflectDir = glm::reflect(-lightDir, normal);
   float spec = glm::pow(glm::max(glm::dot(viewDir, reflectDir), 0.0f), shininess);
   glm::vec3 specular = matSpecular * specularIntensity * spec;

   // Total lighting
   return ambient + diffuse + specular;
}

// Function to calculate vertex colors for Gouraud shading
void calculateVertexColors() {
   vertexColors.clear();
   for (size_t i = 0; i < vertices.size(); i++) {
       glm::vec3 color = computeLighting(vertices[i], normals[i]);
       vertexColors.push_back(color);
   }
}

// Function to render the model with Gouraud shading
void renderModel() {
   glBegin(GL_TRIANGLES);
   for (size_t i = 0; i < indices.size(); i += 3) {
       // Get vertices and normals for the triangle
       glm::vec3 v0 = vertices[indices[i]];
       glm::vec3 v1 = vertices[indices[i + 1]];
       glm::vec3 v2 = vertices[indices[i + 2]];

       glm::vec3 n0 = normals[indices[i]];
       glm::vec3 n1 = normals[indices[i + 1]];
       glm::vec3 n2 = normals[indices[i + 2]];

       // Compute lighting for each vertex
       glm::vec3 color0 = computeLighting(v0, n0);
       glm::vec3 color1 = computeLighting(v1, n1);
       glm::vec3 color2 = computeLighting(v2, n2);

       // Draw the triangle with interpolated colors
       glColor3f(color0.x, color0.y, color0.z);
       glVertex3f(v0.x, v0.y, v0.z);

       glColor3f(color1.x, color1.y, color1.z);
       glVertex3f(v1.x, v1.y, v1.z);

       glColor3f(color2.x, color2.y, color2.z);
       glVertex3f(v2.x, v2.y, v2.z);
   }
   glEnd();
}

// Function to draw a small sphere for the light
void drawLight() {
   glPushMatrix();
   glTranslatef(lightPos.x, lightPos.y, lightPos.z); // Move to light position
   glColor3f(1.0f, 1.0f, 1.0f); // Light color (white)
   glutSolidSphere(10.0f, 30, 30); // Draw a small sphere
   glPopMatrix();
}

void drawInfinitePlane() {
   const float gridSize = 1000.0f; // Size of the grid (extent in each direction)
   const float gridStep = 10.0f;  // Distance between grid lines

   // Enable blending for transparency (optional)
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   // Set the color of the grid lines (light gray)
   glColor4f(0.7f, 0.7f, 0.7f, 0.5f); // RGBA (A = alpha for transparency)

   // Draw the grid
   glBegin(GL_LINES);
   for (float i = -gridSize; i <= gridSize; i += gridStep) {
       // Horizontal lines
       glVertex3f(-gridSize, i, 0.0f);
       glVertex3f(gridSize, i, 0.0f);

       // Vertical lines
       glVertex3f(i, -gridSize, 0.0f);
       glVertex3f(i, gridSize, 0.0f);
   }
   glEnd();

   // Disable blending (if enabled)
   glDisable(GL_BLEND);
}

// Function to handle keyboard input (move the light)
void keyboard(unsigned char key, int x, int y) {
   float moveAmount = 10.0f; // Light movement speed

   switch (key) {
   case 'w': zNear += moveAmount; break; 
   case 's': zNear -= moveAmount; break; 
   case 'a': zFar -= moveAmount; break; 
   case 'd': zFar += moveAmount; break; 
   case '=': FOV += 5.0; break;
   case '-': FOV -= 5.0; break;
   }

   std::cout <<  zNear << ", " << zFar <<  std::endl;
   std::cout << "FOV: " << FOV << std::endl;


   // Recalculate vertex colors for Gouraud shading
   calculateVertexColors();
   // Prevent extreme values
   if (FOV < 10.0f) FOV = 10.0f;
   if (FOV > 120.0f) FOV = 120.0f;

   glutPostRedisplay(); // Redraw the scene
}

// Function to handle mouse motion (rotate model)
void mouseMotion(int x, int y) {
   static int lastX = x, lastY = y;
   int deltaX = x - lastX;
   int deltaY = y - lastY;

   modelRotY += deltaX * 0.5f; // Rotate around Y axis
   modelRotX += deltaY * 0.5f; // Rotate around X axis

   lastX = x;
   lastY = y;

   glutPostRedisplay(); // Redraw the scene
   glutReshapeWindow(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
}

void printMatrix(glm::mat4 matrix) {
   for (int i = 0; i < 4; ++i) { // Rows
       std::cout << " [ " << "\t";
       for (int j = 0; j < 4; ++j) { // Columns
           std::cout << std::fixed << std::showpoint;
           std::cout << std::setprecision(4);
           std::cout << float(matrix[i][j]) << "\t\t"; // Tab-separated values
       }
       std::cout << " ] " << std::endl; // Newline for each row
   }
}

void printProjectionMatrix() {
   // Define the projection matrix (Perspective Projection Example)
   glm::mat4 projection = glm::perspective(FOV, 800.0f / 600.0f, 0.1f, 100.0f);

   // Print projection matrix
   std::cout << "Projection Matrix:\n" << std::endl;

   printMatrix(projection);
}

void printModelViewMatrix() {
   GLfloat modelview[16];
   glGetFloatv(GL_MODELVIEW_MATRIX, modelview);

   glm::mat4 modelViewMat = glm::make_mat4(modelview);
   std::cout << "ModelView Matrix:\n" << std::endl;

   printMatrix(modelViewMat);
}

// Function to render the scene
void display() {
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glLoadIdentity();

   // Set up the camera (fixed camera, move the model instead)
   gluLookAt(10.0f, 50.0f, 100.0f,  // Camera position
       0.0f, 0.0f, 0.0f,  // Look at point
       0.0f, 1.0f, 0.0f); // Up vector

   printProjectionMatrix();
   printModelViewMatrix();

   // Apply model transformations
   glTranslatef(modelX, modelY, modelZ); // Move the model
   glRotatef(modelRotX, 1.0f, 0.0f, 0.0f); // Rotate around X axis
   glRotatef(modelRotY, 0.0f, 1.0f, 0.0f); // Rotate around Y axis
   glRotatef(modelRotZ, 0.0f, 0.0f, 1.0f); // Rotate around Z axis

   drawInfinitePlane();

   // Render the model
   renderModel();

   // Draw the light
   drawLight();

   glutSwapBuffers();
}

// Function to initialize OpenGL settings
void init() {
   glEnable(GL_DEPTH_TEST); // Enable depth testing

   glClearColor(0.0, 0.0, 0.0, 1.0);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(FOV, 800/600.0, 0.1, 1000.0);

   // Load the model
   if (!loadModel("model.stl")) {
       std::cerr << "Failed to load model" << std::endl;
       exit(1);
   }

   // Calculate vertex colors for Gouraud shading
   calculateVertexColors();
}

// Function to handle window resizing
void reshape(int w, int h) {
   glViewport(0, 0, w, h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(FOV, 800 / 600.0, zNear, zFar);  // Apply new FOV

   glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize(800, 600);
   glutCreateWindow("Task2 - Perspective Projection");

   init();

   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutKeyboardFunc(keyboard);
   glutMotionFunc(mouseMotion);

   glutMainLoop();
   return 0;
}