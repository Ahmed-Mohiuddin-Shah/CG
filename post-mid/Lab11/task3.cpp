#include "raylib/raylib.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <raylib/raymath.h>
#include <thread>
#include <mutex>
#include <stdio.h>

// Define the Photon struct
struct Photon
{
    Vector3 position;
    Vector3 velocity;
    Color color;
    float radius = 0.02f;  // Radius of the photon
    bool drawn = false;    // Flag to indicate if the photon is drawn
    bool absorbed = false; // Flag to indicate if the photon is absorbed
    int depth = 0;         // Depth of reflection
};

// Define the Sphere struct
struct Sphere
{
    Vector3 center;
    float radius;
    Color color;
};

// Define the Floor struct
struct Floor
{
    Vector3 position;
    Vector2 size;
    Color color;
};

// Function to check if a photon intersects a sphere
bool CheckPhotonSphereCollision(const Photon &photon, const Sphere &sphere)
{
    float distance = Vector3Distance(photon.position, sphere.center);
    return distance <= sphere.radius;
}

// Function to check if a photon intersects the floor
bool CheckPhotonFloorCollision(const Photon &photon, const Floor &floor)
{
    return photon.position.y <= floor.position.y &&
           photon.position.x >= floor.position.x - floor.size.x / 2 &&
           photon.position.x <= floor.position.x + floor.size.x / 2 &&
           photon.position.z >= floor.position.z - floor.size.y / 2 &&
           photon.position.z <= floor.position.z + floor.size.y / 2;
}

// Function to blend two colors
Color BlendColors(const Color &c1, const Color &c2)
{
    return {
        (c1.r + c2.r) / 2,
        (c1.g + c2.g) / 2,
        (c1.b + c2.b) / 2,
        255 // Keep alpha constant
    };
}

Vector3 ReflectVelocity(const Vector3 &velocity, const Vector3 &normal)
{
    return Vector3Subtract(velocity, Vector3Scale(normal, 2.0f * Vector3DotProduct(velocity, normal)));
}

// Function to update photons in a specific range
void UpdatePhotons(std::vector<Photon> &photons, size_t start, size_t end, const Sphere &redBall, const Sphere &blueBall, const Floor &floor, std::vector<Photon> &absorbedPhotons, int maxDepth, std::vector<Vector3> &lightSources)
{
    for (Vector3 lightSource : lightSources)
    {
        for (size_t i = start; i < end; ++i)
        {
            Photon &photon = photons[i];

            if (photon.absorbed)
            {
                if (photon.depth >= maxDepth)
                {
                    // Reset photon if max depth is reached
                    std::srand(std::time(nullptr) + i); // Change seed for each photon
                    photon.position = lightSource;
                    float theta = float(rand()) / RAND_MAX * 2.0f * PI;       // Random angle in XY plane
                    float phi = acos(2.0f * float(rand()) / RAND_MAX - 1.0f); // Random angle for Z
                    float r = (float(rand()) / RAND_MAX) * 2.0f;              // Random radius within sphere
                    photon.velocity = {
                        r * sin(phi) * cos(theta),
                        r * sin(phi) * sin(theta),
                        r * cos(phi)};
                    photon.color = {255, 255, 255, 100}; // Initially white
                    photon.absorbed = false;
                    photon.radius = 0.02f; // Reset radius
                    photon.depth = 0;      // Reset depth
                    continue;
                }
                photon.color = BlendColors(photon.color, absorbedPhotons.back().color);
                photon.absorbed = false;
                photon.radius *= 0.5f; // Decrease radius
                photon.depth++;
            }

            // Check if photon is out of bounds
            if (photon.position.x < -10.0f || photon.position.x > 10.0f ||
                photon.position.y < -10.0f || photon.position.y > 10.0f ||
                photon.position.z < -10.0f || photon.position.z > 10.0f)
            {
                std::srand(std::time(nullptr) + i);
                photon.position = lightSource;
                float theta = float(rand()) / RAND_MAX * 2.0f * PI;       // Random angle in XY plane
                float phi = acos(2.0f * float(rand()) / RAND_MAX - 1.0f); // Random angle for Z
                float r = (float(rand()) / RAND_MAX) * 2.0f;              // Random radius within sphere
                photon.velocity = {
                    r * sin(phi) * cos(theta),
                    r * sin(phi) * sin(theta),
                    r * cos(phi)};
                photon.color = BLACK;
                photon.absorbed = false;
                photon.radius = 0.02f; // Reset radius
                photon.depth = 0;
                continue;
            }

            photon.position.x += photon.velocity.x * 0.4f;
            photon.position.y += photon.velocity.y * 0.4f;
            photon.position.z += photon.velocity.z * 0.4f;

            // Check collisions with the floor
            if (CheckPhotonFloorCollision(photon, floor))
            {
                photon.color = BlendColors(photon.color, floor.color);
                // reflect with velocity by calculating the reflection with the normal of the surface
                photon.velocity = ReflectVelocity(photon.velocity, {0.0f, 1.0f, 0.0f});
                photon.position.y = floor.position.y;
                photon.absorbed = true;
                absorbedPhotons.push_back(photon);
                continue;
            }

            // Check collisions with the spheres
            if (CheckPhotonSphereCollision(photon, redBall))
            {
                photon.color = BlendColors(photon.color, redBall.color);
                // reflect with velocity by calculating the reflection with the normal of the surface
                Vector3 normal = Vector3Normalize(Vector3Subtract(photon.position, redBall.center));
                photon.velocity = ReflectVelocity(photon.velocity, normal);
                photon.absorbed = true;
                absorbedPhotons.push_back(photon);
                continue;
            }
            else if (CheckPhotonSphereCollision(photon, blueBall))
            {
                photon.color = BlendColors(photon.color, blueBall.color);
                // reflect with velocity by calculating the reflection with the normal of the surface
                Vector3 normal = Vector3Normalize(Vector3Subtract(photon.position, redBall.center));
                photon.velocity = ReflectVelocity(photon.velocity, normal);
                photon.absorbed = true;
                absorbedPhotons.push_back(photon);
                continue;
            }
        }
    }
}

int main()
{
    // Initialize Raylib
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Photon Simulation");
    SetTargetFPS(60);

    // Initialize the scene
    Sphere redBall = {{-2.0f, 1.0f, 0.0f}, 1.0f, RED};
    Sphere blueBall = {{2.0f, 1.0f, 0.0f}, 1.0f, BLUE};
    Floor floor = {{0.0f, 0.0f, 0.0f}, {10.0f, 10.0f}, BEIGE};

    Model ballModel = LoadModelFromMesh(GenMeshSphere(1.0f, 16, 16));

    // Initialize the light source
    std::vector<Vector3> lightSources = {
        {0.0f, 2.0f, 0.0f},
        {0.0f, 2.0f, -2.0f},
        {0.0f, 2.0f, 2.0f}}; // Multiple light sources
    std::vector<Photon> photons;
    std::vector<Photon> absorbedPhotons;

    absorbedPhotons.reserve(10000000);

    // Seed random number generator
    std::srand(std::time(nullptr));

    // Emit photons
    for (Vector3 lightSource : lightSources)
    {
        for (int i = 0; i < 10000; i++)
        {
            Photon photon;
            photon.position = lightSource;
            float theta = float(rand()) / RAND_MAX * 2.0f * PI;       // Random angle in XY plane
            float phi = acos(2.0f * float(rand()) / RAND_MAX - 1.0f); // Random angle for Z
            float r = (float(rand()) / RAND_MAX) * 2.0f;              // Random radius within sphere
            photon.velocity = {
                r * sin(phi) * cos(theta),
                r * sin(phi) * sin(theta),
                r * cos(phi)};
            photon.color = {255, 255, 255, 100};
            photons.push_back(photon);
        }
    }

    const int maxDepth = 10; // Maximum reflection depth

    Camera3D camera;
    camera.position = {0.0f, 5.0f, 10.0f};
    camera.target = {0.0f, 0.0f, 0.0f};
    camera.up = {0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    // Main game loop
    while (!IsKeyPressed(KEY_ONE))
    {
        // Update photons using multiple threads
        const size_t numThreads = std::thread::hardware_concurrency();
        const size_t photonsPerThread = photons.size() / numThreads;
        std::vector<std::thread> threads;

        for (size_t t = 0; t < numThreads; ++t)
        {
            size_t start = t * photonsPerThread;
            size_t end = (t == numThreads - 1) ? photons.size() : start + photonsPerThread;
            threads.emplace_back(UpdatePhotons, std::ref(photons), start, end, std::ref(redBall), std::ref(blueBall), std::ref(floor), std::ref(absorbedPhotons), maxDepth, std::ref(lightSources));
        }

        for (auto &thread : threads)
        {
            thread.join();
        }

        printf("Photons absorbed: %zu\n", absorbedPhotons.size());

        BeginDrawing();

        ClearBackground(BLACK);

        BeginMode3D(camera);

        // Render the light
        for (const auto &lightSource : lightSources)
        {
            DrawModel(ballModel, lightSource, 0.2f, YELLOW);
        }

        // Draw the floor
        DrawCube({floor.position.x, floor.position.y - 0.2f, floor.position.z}, floor.size.x * 0.9f, 0.1f * 0.9f, floor.size.y * 0.9f, {20, 20, 20, 100});

        // Draw the spheres
        DrawModel(ballModel, redBall.center, redBall.radius * 0.9f, {20, 20, 20, 100});
        DrawModel(ballModel, blueBall.center, blueBall.radius * 0.9f, {20, 20, 20, 100});

        // Draw the photons
        for (auto &photon : absorbedPhotons)
        {
            DrawModel(ballModel, photon.position, photon.radius, photon.color);
        }

        EndMode3D();
        EndDrawing();

        // char filename[150];
        // time_t now = time(nullptr);
        // snprintf(filename, sizeof(filename), "./snapshots/screenshot_absorbed_%zu.png", absorbedPhotons.size());
        // TakeScreenshot(filename);
    }

    float radius = 0.02f;

    while (!IsKeyPressed(KEY_TWO))
    {

        if (IsKeyDown(KEY_UP))
            radius -= 0.01f;
        if (IsKeyDown(KEY_DOWN))
            radius += 0.01f;

        UpdateCamera(&camera, CAMERA_FIRST_PERSON);

        BeginDrawing();
        ClearBackground(BLACK);

        BeginMode3D(camera);

        // Render the light
        for (const auto &lightSource : lightSources)
        {
            DrawModel(ballModel, lightSource, 0.2f, YELLOW);
        }

        // Draw the floor
        DrawCube({floor.position.x, floor.position.y - 0.2f, floor.position.z}, floor.size.x * 0.9f, 0.1f * 0.9f, floor.size.y * 0.9f, {20, 20, 20, 100});

        // Draw the spheres
        DrawModel(ballModel, redBall.center, redBall.radius * 0.9f, {20, 20, 20, 100});
        DrawModel(ballModel, blueBall.center, blueBall.radius * 0.9f, {20, 20, 20, 100});

        // Draw the absorbed photons
        for (auto &photon : absorbedPhotons)
        {
            DrawModel(ballModel, photon.position, radius, photon.color);
        }

        EndMode3D();

        DrawText("Use WASD to move, mouse to look around, and ESC to exit.", 10, 10, 20, WHITE);

        EndDrawing();
    }

    // Close Raylib
    CloseWindow();

    return 0;
}