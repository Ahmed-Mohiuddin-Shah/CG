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
    bool absorbed = false; // Flag to indicate if the photon is absorbed
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

// Function to update photons in a specific range
void UpdatePhotons(std::vector<Photon> &photons, size_t start, size_t end, const Sphere &redBall, const Sphere &blueBall, const Floor &floor, std::vector<Photon> &absorbedPhotons)
{

    for (size_t i = start; i < end; ++i)
    {
        Photon &photon = photons[i];

        if (photon.absorbed)
        {
            // change seed
            std::srand(std::time(nullptr) + i); // Change seed for each photon
            photon.position = {0.0f, 2.0f, 0.0f};
            photon.velocity = {
                (float(rand()) / RAND_MAX - 0.5f) * 0.2f,  // Increased randomness
                (float(rand()) / RAND_MAX - 0.5f) * 0.2f,  // Increased randomness
                (float(rand()) / RAND_MAX - 0.5f) * 0.2f}; // Increased randomness
            photon.color = BLACK;                          // Initially black
            photon.absorbed = false;                       // Reset absorbed state
        }

        // check if photon out of bounds
        if (photon.position.x < -10.0f || photon.position.x > 10.0f ||
            photon.position.y < -10.0f || photon.position.y > 10.0f ||
            photon.position.z < -10.0f || photon.position.z > 10.0f)
        {
            // change seed
            std::srand(std::time(nullptr) + i); // Change seed for each photon
            photon.position = {0.0f, 2.0f, 0.0f};
            photon.velocity = {
                (float(rand()) / RAND_MAX - 0.5f) * 0.5f,  // Increased randomness
                (float(rand()) / RAND_MAX - 0.5f) * 0.5f,  // Increased randomness
                (float(rand()) / RAND_MAX - 0.5f) * 0.5f}; // Increased randomness
            photon.color = BLACK;                          // Initially black
            photon.absorbed = false;                       // Reset absorbed state
        }

        photon.position.x += photon.velocity.x * 0.1f;
        photon.position.y += photon.velocity.y * 0.1f;
        photon.position.z += photon.velocity.z * 0.1f;

        // Check collisions with the floor
        if (CheckPhotonFloorCollision(photon, floor))
        {
            photon.color = floor.color;
            photon.velocity = {0.0f, 0.0f, 0.0f}; // Stop the photon
            photon.position.y = floor.position.y; // Set photon on the floor
            photon.absorbed = true;               // Mark photon as absorbed
            absorbedPhotons.push_back(photon);    // Store the absorbed photon
            continue;                             // Skip further processing
        }

        // Check collisions with the spheres
        if (CheckPhotonSphereCollision(photon, redBall))
        {
            photon.color = redBall.color;
            photon.velocity = {0.0f, 0.0f, 0.0f}; // Stop the photon
            photon.absorbed = true;               // Mark photon as absorbed
            absorbedPhotons.push_back(photon);    // Store the absorbed photon

            continue; // Skip further processing
        }
        else if (CheckPhotonSphereCollision(photon, blueBall))
        {
            photon.color = blueBall.color;
            photon.velocity = {0.0f, 0.0f, 0.0f}; // Stop the photon
            photon.absorbed = true;               // Mark photon as absorbed
            absorbedPhotons.push_back(photon);    // Store the absorbed photon
            continue;                             // Skip further processing
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
    Floor floor = {{0.0f, 0.0f, 0.0f}, {10.0f, 10.0f}, GRAY};

    // Initialize the light source
    Vector3 lightSource = {0.0f, 2.0f, 0.0f};
    std::vector<Photon> photons;
    std::vector<Photon> absorbedPhotons;
    absorbedPhotons.reserve(100000); // Reserve space for absorbed photons

    // Seed random number generator
    std::srand(std::time(nullptr));

    // Emit photons
    for (int i = 0; i < 5000; i++)
    {
        Photon photon;
        photon.position = lightSource;
        photon.velocity = {
            (float(rand()) / RAND_MAX - 0.5f) * 2.0f,
            (float(rand()) / RAND_MAX - 0.5f) * 2.0f,
            (float(rand()) / RAND_MAX - 0.5f) * 2.0f};
        photon.color = BLACK; // Initially black
        photons.push_back(photon);
    }

    // Main game loop
    while (!WindowShouldClose())
    {
        // Update photons using multiple threads
        const size_t numThreads = std::thread::hardware_concurrency();
        const size_t photonsPerThread = photons.size() / numThreads;
        std::vector<std::thread> threads;

        for (size_t t = 0; t < numThreads; ++t)
        {
            size_t start = t * photonsPerThread;
            size_t end = (t == numThreads - 1) ? photons.size() : start + photonsPerThread;
            threads.emplace_back(UpdatePhotons, std::ref(photons), start, end, std::ref(redBall), std::ref(blueBall), std::ref(floor), std::ref(absorbedPhotons));
        }

        for (auto &thread : threads)
        {
            thread.join();
        }

        printf("Photons absorbed: %zu\n", absorbedPhotons.size());

        // Draw the scene
        BeginDrawing();

        // Retain and draw on top of the previous scene
        ClearBackground(BLACK); // Use BLANK to retain the previous frame

        BeginMode3D(Camera3D{{0.0f, 5.0f, 10.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, 45.0f, 0});

        // render the light
        DrawSphere(lightSource, 0.2f, YELLOW);

        // Draw the photons
        for (const auto &photon : absorbedPhotons)
        {
            DrawSphere(photon.position, 0.09f, photon.color);
        }

        EndMode3D();
        EndDrawing();
    }

    // Close Raylib
    CloseWindow();

    return 0;
}