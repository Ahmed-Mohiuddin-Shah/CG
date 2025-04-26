#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <algorithm>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;

// Constants
const int WIDTH = 800;
const int HEIGHT = 600;
const int PHOTONS_TO_EMIT = 10000;
const float PHOTON_SEARCH_RADIUS = 0.5f;
const int MAX_PHOTONS_TO_USE = 100;

// Structures
struct Photon
{
    vec3 position;
    vec3 power;
    vec3 direction;
};

struct Ray
{
    vec3 origin;
    vec3 direction;
};

struct Sphere
{
    vec3 center;
    float radius;
    vec3 color;
    bool reflective;
    bool refractive;
    float refractiveIndex;
};

struct HitRecord
{
    float t;
    vec3 point;
    vec3 normal;
    vec3 color;
    bool reflective;
    bool refractive;
    float refractiveIndex;
};

// Global variables
vector<Photon> photonMap;
vector<Sphere> spheres;
vec3 lightPosition;
vec3 lightPower;
GLuint VAO, VBO, shaderProgram;
vector<vec3> pixels(WIDTH *HEIGHT);

// Random number generation
random_device rd;
mt19937 gen(rd());
uniform_real_distribution<float> dist(0.0f, 1.0f);

// Shader sources
const char *vertexShaderSource = R"glsl(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;
    out vec3 ourColor;
    void main() {
        gl_Position = vec4(aPos, 1.0);
        ourColor = aColor;
    }
)glsl";

const char *fragmentShaderSource = R"glsl(
    #version 330 core
    in vec3 ourColor;
    out vec4 FragColor;
    void main() {
        FragColor = vec4(ourColor, 1.0);
    }
)glsl";

// Function declarations
void initScene();
bool intersectSphere(const Ray &ray, const Sphere &sphere, float t_min, float t_max, HitRecord &rec);
bool traceRay(const Ray &ray, float t_min, float t_max, HitRecord &rec);
vec3 tracePhotons(int numPhotons);
void emitPhotons();
vec3 estimateRadiance(const vec3 &point, const vec3 &normal);
void renderScene();
void setupOpenGL();
void display();

// Main function
int main()
{
    if (!glfwInit())
    {
        cerr << "Failed to initialize GLFW" << endl;
        return -1;
    }

    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Task 1 - Implement Basic Photon Mapping", nullptr, nullptr);
    if (!window)
    {
        cerr << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        cerr << "Failed to initialize GLEW" << endl;
        return -1;
    }

    setupOpenGL();
    initScene();
    emitPhotons();
    renderScene();

    while (!glfwWindowShouldClose(window))
    {
        display();
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void initScene()
{
    // Light setup
    lightPosition = vec3(0.0f, 5.0f, 0.0f);
    lightPower = vec3(300.0f, 300.0f, 300.0f);

    // Scene objects
    spheres.push_back({vec3(0.0f, -1000.5f, -1.0f), 1000.0f, vec3(0.8f, 0.8f, 0.8f), false, false, 1.0f}); // Floor
    spheres.push_back({vec3(0.0f, 0.0f, -1.0f), 0.5f, vec3(0.8f, 0.3f, 0.3f), false, false, 1.0f});        // Red sphere
    spheres.push_back({vec3(-1.0f, 0.0f, -1.0f), 0.5f, vec3(0.8f, 0.8f, 0.8f), true, false, 1.0f});        // Mirror sphere
    spheres.push_back({vec3(1.0f, 0.0f, -1.0f), 0.5f, vec3(0.8f, 0.8f, 0.8f), false, true, 1.5f});         // Glass sphere
}

bool intersectSphere(const Ray &ray, const Sphere &sphere, float t_min, float t_max, HitRecord &rec)
{
    vec3 oc = ray.origin - sphere.center;
    float a = dot(ray.direction, ray.direction);
    float b = 2.0f * dot(oc, ray.direction);
    float c = dot(oc, oc) - sphere.radius * sphere.radius;
    float discriminant = b * b - 4 * a * c;

    if (discriminant > 0)
    {
        float temp = (-b - sqrt(discriminant)) / (2.0f * a);
        if (temp < t_max && temp > t_min)
        {
            rec.t = temp;
            rec.point = ray.origin + rec.t * ray.direction;
            rec.normal = (rec.point - sphere.center) / sphere.radius;
            rec.color = sphere.color;
            rec.reflective = sphere.reflective;
            rec.refractive = sphere.refractive;
            rec.refractiveIndex = sphere.refractiveIndex;
            return true;
        }
        temp = (-b + sqrt(discriminant)) / (2.0f * a);
        if (temp < t_max && temp > t_min)
        {
            rec.t = temp;
            rec.point = ray.origin + rec.t * ray.direction;
            rec.normal = (rec.point - sphere.center) / sphere.radius;
            rec.color = sphere.color;
            rec.reflective = sphere.reflective;
            rec.refractive = sphere.refractive;
            rec.refractiveIndex = sphere.refractiveIndex;
            return true;
        }
    }
    return false;
}

bool traceRay(const Ray &ray, float t_min, float t_max, HitRecord &rec)
{
    HitRecord temp_rec;
    bool hit_anything = false;
    float closest_so_far = t_max;

    for (const auto &sphere : spheres)
    {
        if (intersectSphere(ray, sphere, t_min, closest_so_far, temp_rec))
        {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }

    return hit_anything;
}

vec3 tracePhotons(int numPhotons)
{
    vec3 accumulatedPower(0.0f);
    for (int i = 0; i < numPhotons; ++i)
    {
        // Random direction on hemisphere
        float theta = 2.0f * M_PI * dist(gen);
        float phi = acos(1.0f - 2.0f * dist(gen));
        vec3 direction(sin(phi) * cos(theta), cos(phi), sin(phi) * sin(theta));

        Ray ray{lightPosition, direction};
        vec3 power = lightPower / float(numPhotons);
        bool specularBounce = false;

        for (int depth = 0; depth < 5; ++depth)
        {
            HitRecord rec;
            if (!traceRay(ray, 0.001f, 10000.0f, rec))
                break;

            // Store photon on diffuse surfaces
            if (!specularBounce && (!rec.reflective && !rec.refractive))
            {
                photonMap.push_back({rec.point, power, -ray.direction});
            }

            // Russian roulette termination
            if (depth > 3)
            {
                float continueProbability = std::min(1.0f, std::max(std::max(power.x, power.y), power.z) * 10.0f);
                if (dist(gen) > continueProbability)
                    break;
                power /= continueProbability;
            }

            // Diffuse reflection
            if (!rec.reflective && !rec.refractive)
            {
                // Sample random direction on hemisphere
                float r1 = 2.0f * M_PI * dist(gen);
                float r2 = dist(gen);
                float r2s = sqrt(r2);

                vec3 w = rec.normal;
                vec3 u = normalize(cross((fabs(w.x) > 0.1f ? vec3(0.0f, 1.0f, 0.0f) : vec3(1.0f, 0.0f, 0.0f)), w));
                vec3 v = cross(w, u);

                vec3 d = normalize(u * cos(r1) * r2s + v * sin(r1) * r2s + w * sqrt(1.0f - r2));

                power *= rec.color;
                ray = Ray{rec.point + rec.normal * 0.001f, d};
                specularBounce = false;
            }
            // Specular reflection
            else if (rec.reflective)
            {
                vec3 reflected = reflect(ray.direction, rec.normal);
                ray = Ray{rec.point + rec.normal * 0.001f, reflected};
                specularBounce = true;
            }
            // Refraction
            else if (rec.refractive)
            {
                float eta = (rec.refractiveIndex > 1.0f) ? 1.0f / rec.refractiveIndex : rec.refractiveIndex;
                vec3 refracted = refract(ray.direction, rec.normal, eta);

                if (length(refracted) < 0.001f)
                { // Total internal reflection
                    refracted = reflect(ray.direction, rec.normal);
                }

                ray = Ray{rec.point - rec.normal * 0.001f, refracted};
                specularBounce = true;
            }
        }
    }
    return accumulatedPower;
}

void emitPhotons()
{
    cout << "Emitting photons..." << endl;
    tracePhotons(PHOTONS_TO_EMIT);
    cout << "Photon map contains " << photonMap.size() << " photons" << endl;
}

vec3 estimateRadiance(const vec3 &point, const vec3 &normal)
{
    vec3 radiance(0.0f);
    int photonsFound = 0;
    float maxDist2 = PHOTON_SEARCH_RADIUS * PHOTON_SEARCH_RADIUS;

    for (const auto &photon : photonMap)
    {
        vec3 diff = photon.position - point;
        float dist2 = dot(diff, diff);
        if (dist2 < maxDist2 && dot(photon.direction, normal) > 0.0f)
        {
            radiance += photon.power;
            photonsFound++;
            if (photonsFound >= MAX_PHOTONS_TO_USE)
                break;
        }
    }

    if (photonsFound > 0)
    {
        float area = M_PI * maxDist2;
        radiance /= (area * photonsFound);
    }

    return radiance;
}

void renderScene()
{
    cout << "Rendering scene..." << endl;
    vec3 eye(0.0f, 2.0f, 3.0f);
    vec3 lookAt(0.0f, 0.0f, -1.0f);
    vec3 up(0.0f, 1.0f, 0.0f);
    float fov = 45.0f;

    vec3 w = normalize(eye - lookAt);
    vec3 u = normalize(cross(up, w));
    vec3 v = cross(w, u);

    float aspect = float(WIDTH) / float(HEIGHT);
    float halfHeight = tan(fov * M_PI / 360.0f);
    float halfWidth = aspect * halfHeight;

    for (int y = 0; y < HEIGHT; ++y)
    {
        for (int x = 0; x < WIDTH; ++x)
        {
            float u_coord = (float(x) + 0.5f) / float(WIDTH) * 2.0f - 1.0f;
            float v_coord = 1.0f - (float(y) + 0.5f) / float(HEIGHT) * 2.0f;

            vec3 rd = normalize(u * u_coord * halfWidth + v * v_coord * halfHeight - w);
            Ray ray{eye, rd};

            HitRecord rec;
            if (traceRay(ray, 0.001f, 10000.0f, rec))
            {
                // Direct illumination (simple for demonstration)
                vec3 lightDir = normalize(lightPosition - rec.point);
                float lightDistance = length(lightPosition - rec.point);
                Ray shadowRay{rec.point + rec.normal * 0.001f, lightDir};
                HitRecord shadowRec;

                vec3 direct = vec3(0.0f);
                if (!traceRay(shadowRay, 0.001f, lightDistance, shadowRec))
                {
                    float cosine = std::max(0.0f, dot(rec.normal, lightDir));
                    float denominator = 4.0f * M_PI * lightDistance * lightDistance;
                    direct = vec3(lightPower.x * cosine / denominator,
                                  lightPower.y * cosine / denominator,
                                  lightPower.z * cosine / denominator);
                }

                // Indirect illumination from photon map
                vec3 indirect = estimateRadiance(rec.point, rec.normal);

                // Combine with surface color
                pixels[y * WIDTH + x] = rec.color * (direct + indirect);
            }
            else
            {
                pixels[y * WIDTH + x] = vec3(0.1f, 0.1f, 0.3f); // Background color
            }
        }
    }
}

void setupOpenGL()
{
    // Shader setup
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // VAO/VBO setup
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Convert pixels to OpenGL format
    vector<float> vertices;
    for (int y = 0; y < HEIGHT; ++y)
    {
        for (int x = 0; x < WIDTH; ++x)
        {
            vec3 color = pixels[y * WIDTH + x];

            // Convert pixel coordinates to NDC [-1, 1]
            float xpos = (2.0f * x / WIDTH) - 1.0f;
            float ypos = 1.0f - (2.0f * y / HEIGHT);

            // Add vertex position and color
            vertices.push_back(xpos);
            vertices.push_back(ypos);
            vertices.push_back(0.0f);
            vertices.push_back(color.r);
            vertices.push_back(color.g);
            vertices.push_back(color.b);
        }
    }

    // Update VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Draw
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, WIDTH * HEIGHT);
    glBindVertexArray(0);

    glfwSwapBuffers(glfwGetCurrentContext());
}