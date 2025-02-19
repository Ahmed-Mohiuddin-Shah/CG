#include <GL/glew.h>
#include <GL/freeglut.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <iostream>
#include <cmath>
#include <glm/glm.hpp>
#include <fstream>
#include <sstream>
#include <string>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>

// Model transformation variables-155, -40, -100,
float modelX = -155.0f, modelY = -40.0f, modelZ = -100.0f;      // Model position
float modelRotX = -60.0f, modelRotY = 0.0f, modelRotZ = -30.0f; // Model rotation

// Light properties
glm::vec3 lightPos = glm::vec3(170.0f, 80.0f, 80.0f); // Light position
glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);   // Light color (white)
float ambientIntensity = 0.2f;                        // Ambient intensity
float diffuseIntensity = 0.8f;                        // Diffuse intensity
float specularIntensity = 1.0f;                       // Specular intensity
float shininess = 50.0f;                              // Shininess exponent

// Material properties (model color is red)
glm::vec3 matAmbient = glm::vec3(0.7f, 0.0f, 0.0f);  // Ambient reflection (red)
glm::vec3 matDiffuse = glm::vec3(0.8f, 0.0f, 0.0f);  // Diffuse reflection (red)
glm::vec3 matSpecular = glm::vec3(0.5f, 0.5f, 0.5f); // Specular reflection (white)

// Model data
std::vector<glm::vec3> vertices;
std::vector<glm::vec3> normals;
std::vector<unsigned int> indices;
std::vector<glm::vec3> vertexColors; // Colors for Gouraud shading

GLuint shaderProgram;

// Function to read a shader file
std::string readShaderFile(const std::string &filePath)
{
    std::ifstream file(filePath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Function to compile a shader
GLuint compileShader(GLenum type, const std::string &source)
{
    GLuint shader = glCreateShader(type);
    const char *src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    // Check for errors
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation error: " << infoLog << std::endl;
        return 0;
    }

    return shader;
}

// Function to create a shader program
GLuint createShaderProgram(const std::string &vertexShaderSource, const std::string &fragmentShaderSource)
{
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check for linking errors
    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Shader program linking error: " << infoLog << std::endl;
        return 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

// Function to load a 3D model using Assimp
bool loadModel(const std::string &path)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "Error loading model: " << importer.GetErrorString() << std::endl;
        return false;
    }

    // Process the first mesh (for simplicity)
    aiMesh *mesh = scene->mMeshes[0];

    // Extract vertices and normals
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        // Vertices
        vertices.push_back(glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));

        // Normals
        if (mesh->HasNormals())
        {
            normals.push_back(glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z));
        }
        else
        {
            // If no normals are present, add a default normal (e.g., facing up)
            normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
        }
    }

    // Extract indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    return true;
}

// Function to compute lighting for a vertex
glm::vec3 computeLighting(const glm::vec3 &vertex, const glm::vec3 &normal)
{
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

GLuint vao, vbo, ebo, nbo;

void renderModel()
{
    // Bind VAO
    glBindVertexArray(vao);

    // Draw the model
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    // Unbind VAO
    glBindVertexArray(0);
}

// Function to draw a small sphere for the light
void drawLight()
{
    glPushMatrix();
    glTranslatef(lightPos.x, lightPos.y, lightPos.z); // Move to light position
    glColor3f(1.0f, 1.0f, 1.0f);                      // Light color (white)
    glutSolidSphere(10.0f, 30, 30);                   // Draw a small sphere
    glPopMatrix();
}

void drawInfinitePlane()
{
    const float gridSize = 1000.0f; // Size of the grid (extent in each direction)
    const float gridStep = 10.0f;   // Distance between grid lines

    // Enable blending for transparency (optional)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set the color of the grid lines (light gray)
    glColor4f(0.7f, 0.7f, 0.7f, 0.5f); // RGBA (A = alpha for transparency)

    // Draw the grid
    glBegin(GL_LINES);
    for (float i = -gridSize; i <= gridSize; i += gridStep)
    {
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
void keyboard(unsigned char key, int x, int y)
{
    float moveAmount = 10.0f; // Light movement speed

    switch (key)
    {
    case 'w':
        lightPos.x += moveAmount;
        break; // Move light forward
    case 's':
        lightPos.x -= moveAmount;
        break; // Move light backward
    case 'a':
        lightPos.y -= moveAmount;
        break; // Move light left
    case 'd':
        lightPos.y += moveAmount;
        break; // Move light right
    case 'q':
        lightPos.z += moveAmount;
        break; // Move light up
    case 'e':
        lightPos.z -= moveAmount;
        break; // Move light down
    }

    std::cout << lightPos.x << ", " << lightPos.y << ", " << lightPos.z << ", " << std::endl;

    //// Recalculate vertex colors for Gouraud shading
    // calculateVertexColors();

    glutPostRedisplay(); // Redraw the scene
}

// Function to handle mouse motion (rotate model)
void mouseMotion(int x, int y)
{
    static int lastX = x, lastY = y;
    int deltaX = x - lastX;
    int deltaY = y - lastY;

    modelRotY += deltaX * 0.5f; // Rotate around Y axis
    modelRotX += deltaY * 0.5f; // Rotate around X axis

    lastX = x;
    lastY = y;

    glutPostRedisplay(); // Redraw the scene
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use the shader program for the model
    glUseProgram(shaderProgram);

    // Set up matrices using glm
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(modelX, modelY, modelZ));
    model = glm::rotate(model, glm::radians(modelRotX), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(modelRotY), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(modelRotZ), glm::vec3(0.0f, 0.0f, 1.0f));

    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 projection = glm::perspective(glm::radians(60.0f), 800.0f / 600.0f, 0.1f, 100000.0f);

    // Pass matrices to shader
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // Pass light and material properties
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, glm::value_ptr(lightPos));
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightColor"), 1, glm::value_ptr(lightColor));
    glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 5.0f)));
    glUniform3fv(glGetUniformLocation(shaderProgram, "matAmbient"), 1, glm::value_ptr(matAmbient));
    glUniform3fv(glGetUniformLocation(shaderProgram, "matDiffuse"), 1, glm::value_ptr(matDiffuse));
    glUniform3fv(glGetUniformLocation(shaderProgram, "matSpecular"), 1, glm::value_ptr(matSpecular));
    glUniform1f(glGetUniformLocation(shaderProgram, "shininess"), shininess);

    // Render the model
    renderModel();

    // Switch to fixed-function pipeline for grid and light
    glUseProgram(0); // Disable shader program

    // Set up the fixed-function pipeline camera
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0f, 800.0f / 600.0f, 0.1f, 100000.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0f, 0.0f, 5.0f,  // Camera position
              0.0f, 0.0f, 0.0f,  // Look at point
              0.0f, 1.0f, 0.0f); // Up vector

    // Render the grid
    drawInfinitePlane();

    // Render the light
    drawLight();

    glutSwapBuffers();
}

// Function to initialize OpenGL settings
void init()
{
    glEnable(GL_DEPTH_TEST); // Enable depth testing

    // Load shaders
    std::string vertexShaderSource = readShaderFile("vertex_shader.glsl");
    std::string fragmentShaderSource = readShaderFile("fragment_shader.glsl");
    shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);

    // Load the model
    if (!loadModel("model.stl"))
    {
        std::cerr << "Failed to load model" << std::endl;
        exit(1);
    }

    // Create VAO, VBO, and EBO
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glGenBuffers(1, &nbo);

    glBindVertexArray(vao);

    // Bind and fill VBO with vertex data
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    // Bind and fill EBO with index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Set up the vertex attribute pointer for vertex positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
    glEnableVertexAttribArray(0);

    // Bind the normal buffer object
    glBindBuffer(GL_ARRAY_BUFFER, nbo); // Assuming you created a separate buffer for normals
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);

    // Set up the vertex attribute pointer for normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
    glEnableVertexAttribArray(1);

    // Bind the element buffer object (EBO) for indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Unbind the VAO
    glBindVertexArray(0);
}

// Function to handle window resizing
void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0f, (float)w / (float)h, 0.1f, 10000.0f); // Set perspective projection
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    // Request a core profile OpenGL context (version 3.3 or later)
    glutInitContextVersion(3, 3);
    glutInitContextProfile(GLUT_CORE_PROFILE);

    glutInitWindowSize(800, 600);
    glutCreateWindow("Task2 - Phong Shading");

    // Initialize GLEW (to load modern OpenGL functions)
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // Check OpenGL version
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    init(); // Call your initialization function

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMotionFunc(mouseMotion);

    glutMainLoop();
    return 0;
}