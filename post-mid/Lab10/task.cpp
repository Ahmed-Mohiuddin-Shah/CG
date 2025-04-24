#define RAYGUI_IMPLEMENTATION

#include <raylib/raylib.h>
#include <raylib/raygui.h>
#include <raylib/raymath.h>
#include <vector>
#include <cmath>
#include <algorithm>
#include <cfloat>

// Constants
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const char *TITLE = "Physics Simulation Sandbox";

// Water simulation constants
int WATER_SIZE = 300;
float WATER_DAMPING = 0.996f;
float WATER_SPREAD = 0.25f;
float WATER_HEIGHT = 0.5f;
float FORCE = 1.0f;

// Particle constants
int MAX_PARTICLES = 2000;
float PARTICLE_LIFETIME = 3.0f;

float GRAVITY = -9.8f;
float DT = 0.016f; // Time step
int SOLVER_ITERATIONS = 10;
float GROUND_Y = -10.0f; // Ground plane height
float MOUSE_INFLUENCE = 10.0f;
float MOUSE_CUT_DISTANCE = 10.0f;

float tempWaterSize = (float)WATER_SIZE;
float tempWaterDamping = WATER_DAMPING;
float tempWaterSpread = WATER_SPREAD;
float tempWaterHeight = WATER_HEIGHT;
float tempForce = FORCE;
float tempMaxParticles = (float)MAX_PARTICLES;
float tempParticleLifetime = PARTICLE_LIFETIME;
float tempGravity = GRAVITY;
float tempDT = DT;
float tempSolverIterations = (float)SOLVER_ITERATIONS;
float tempGroundY = GROUND_Y;
float tempMouseInfluence = MOUSE_INFLUENCE;
float tempMouseCutDistance = MOUSE_CUT_DISTANCE;

// Simulation modes
enum SimulationMode
{
    MODE_WATER,
    MODE_CLOTH,
    MODE_PARTICLES
};
SimulationMode currentMode = MODE_WATER;

struct WaterSimulation
{
    Model waterModel;
    Texture2D heightMap;
    Image heightMapImage;
    float waveTime;
    std::vector<std::vector<float>> heights;
    std::vector<std::vector<float>> velocities;

    void Init()
    {
        // Create a heightmap for the water
        heightMapImage = GenImageColor(WATER_SIZE, WATER_SIZE, BLUE);
        heightMap = LoadTextureFromImage(heightMapImage);

        // Create a simple plane model for the water
        waterModel = LoadModelFromMesh(GenMeshPlane(20, 20, WATER_SIZE - 1, WATER_SIZE - 1));
        waterModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = heightMap;

        waveTime = 0.0f;

        // Initialize height and velocity arrays
        heights.resize(WATER_SIZE, std::vector<float>(WATER_SIZE, WATER_HEIGHT));
        velocities.resize(WATER_SIZE, std::vector<float>(WATER_SIZE, 0.0f));
    }

    void Update()
    {
        // Update wave simulation
        for (int y = 1; y < WATER_SIZE - 1; y++)
        {
            for (int x = 1; x < WATER_SIZE - 1; x++)
            {
                float heightDiff =
                    (heights[y - 1][x] + heights[y + 1][x] + heights[y][x - 1] + heights[y][x + 1] - 4 * heights[y][x]);
                velocities[y][x] += heightDiff * WATER_SPREAD;
                velocities[y][x] *= WATER_DAMPING;
            }
        }

        for (int y = 0; y < WATER_SIZE; y++)
        {
            for (int x = 0; x < WATER_SIZE; x++)
            {
                heights[y][x] += velocities[y][x];
            }
        }

        // Update heightmap texture
        for (int y = 0; y < WATER_SIZE; y++)
        {
            for (int x = 0; x < WATER_SIZE; x++)
            {
                float normalizedHeight = (heights[y][x] - WATER_HEIGHT) * 10.0f + 0.5f;
                normalizedHeight = Clamp(normalizedHeight, 0.25f, 1.0f);
                Color color = {0, 0, (unsigned char)(normalizedHeight * 255), 255};
                ImageDrawPixel(&heightMapImage, x, y, color);
            }
        }

        UpdateTexture(heightMap, heightMapImage.data);

        // Update water model mesh
        UnloadModel(waterModel);
        waterModel = LoadModelFromMesh(GenMeshHeightmap(heightMapImage, {20, WATER_HEIGHT, 20}));
        waterModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = heightMap;
    }

    void AddSplash(int x, int y, float force)
    {
        if (x >= 0 && x < WATER_SIZE && y >= 0 && y < WATER_SIZE)
        {
            velocities[y][x] += force;
        }
    }

    void Draw()
    {
        // Draw the water model
        DrawModel(waterModel, {-10, 0, -10}, 1.0f, BLUE);
    }
};
;

struct Ball
{
    Vector3 position;
    Vector3 velocity;
    float radius;
    bool is_selected;

    Ball(Vector3 pos, float r) : position(pos), radius(r), is_selected(false)
    {
        velocity = {0, 0, 0};
    }

    void update()
    {
        // Update ball position based on gravity
        if (!is_selected)
        {
            velocity.y += GRAVITY;
            position = Vector3Add(position, Vector3Scale(velocity, DT));
            if (position.y < GROUND_Y + radius)
            {
                position.y = GROUND_Y + radius;
                velocity.y *= -0.8f; // Bounce with some energy loss
            }
        }
    }

    void handleMouse()
    {
        if (IsKeyDown(KEY_UP))
        {
            position.z += 0.1f;
        }
        if (IsKeyDown(KEY_DOWN))
        {
            position.z -= 0.1f;
        }
        if (IsKeyDown(KEY_LEFT))
        {
            position.x -= 0.1f;
        }
        if (IsKeyDown(KEY_RIGHT))
        {
            position.x += 0.1f;
        }
        if (IsKeyDown(KEY_SPACE))
        {
            position.y += 0.1f;
        }
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            Vector2 mousePos = GetMousePosition();
            Ray mouseRay = GetMouseRay(mousePos, {0, 0, 0});
            Vector3 to_ball = Vector3Subtract(position, mouseRay.position);
            float distance = Vector3Length(to_ball);
            if (distance < radius)
            {
                is_selected = true;
                position = mouseRay.position + Vector3Scale(mouseRay.direction, radius);
            }
        }
        else
        {
            is_selected = false;
        }
    }

    void draw()
    {
        if (is_selected)
        {
            DrawSphere(position, radius, YELLOW);
        }
        else
        {
            DrawSphere(position, radius, BLUE);
        }
    }
};

// Particle structure
struct ClothParticle
{
    Vector3 pos;
    Vector3 prev_pos;
    Vector3 velocity;
    Vector3 acceleration;
    float mass;
    bool fixed = false;
    bool is_selected;
};

// Spring structure
struct Spring
{
    int p1, p2;
    float rest_length;
    float stiffness;
    bool is_active;
};

// Cloth simulation class
class Cloth
{
public:
    std::vector<ClothParticle> particles;
    std::vector<Spring> springs;
    int width_particles;
    int height_particles;
    float particle_spacing;
    Vector3 mouse_force;
    bool mouse_down;
    Vector3 mouse_pos;

    Cloth(int w, int h, float spacing, bool fixed = true) : width_particles(w), height_particles(h), particle_spacing(spacing),
                                                            mouse_force({0, 0, 0}), mouse_down(false), mouse_pos({0, 0, 0})
    {
        // Create particles
        for (int y = 0; y < height_particles; y++)
        {
            for (int x = 0; x < width_particles; x++)
            {
                ClothParticle p;
                p.pos = Vector3{x * spacing, -y * spacing, 0};
                p.prev_pos = p.pos;
                p.velocity = Vector3{0, 0, 0};
                p.acceleration = Vector3{0, 0, 0};
                p.mass = 1.0f;
                if (fixed)
                {
                    p.fixed = (y == 0 && (x == 0 || x == width_particles - 1)); // Fix top corners
                }
                p.is_selected = false;
                particles.push_back(p);
            }
        }

        // Create structural springs
        for (int y = 0; y < height_particles; y++)
        {
            for (int x = 0; x < width_particles; x++)
            {
                int idx = y * width_particles + x;

                // Horizontal springs
                if (x < width_particles - 1)
                {
                    springs.push_back({idx, idx + 1, particle_spacing, 100.0f, true});
                }

                // Vertical springs
                if (y < height_particles - 1)
                {
                    springs.push_back({idx, idx + width_particles, particle_spacing, 100.0f, true});
                }

                // Diagonal springs
                if (x < width_particles - 1 && y < height_particles - 1)
                {
                    float diag_length = particle_spacing * sqrt(2.0f);
                    springs.push_back({idx, idx + width_particles + 1, diag_length, 50.0f, true});
                    springs.push_back({idx + 1, idx + width_particles, diag_length, 50.0f, true});
                }
            }
        }
    }

    void update()
    {
        // Apply forces
        for (auto &p : particles)
        {
            if (p.fixed)
                continue;

            // Gravity
            p.acceleration = Vector3{0, GRAVITY, 0};

            // Mouse interaction
            if (p.is_selected && mouse_down)
            {
                Vector3 to_mouse = mouse_pos - p.pos;
                float distance = Vector3Length(to_mouse);
                if (distance > 0.01f)
                {
                    p.acceleration = p.acceleration + Vector3Normalize(to_mouse) * MOUSE_INFLUENCE;
                }
            }

            // Verlet integration
            Vector3 temp = p.pos;
            p.pos = p.pos + (p.pos - p.prev_pos) + p.acceleration * DT * DT;
            p.prev_pos = temp;
        }

        // Solve constraints
        for (int i = 0; i < SOLVER_ITERATIONS; i++)
        {
            // Spring constraints
            for (auto &s : springs)
            {
                if (!s.is_active)
                    continue;

                ClothParticle &p1 = particles[s.p1];
                ClothParticle &p2 = particles[s.p2];

                Vector3 delta = p2.pos - p1.pos;
                float distance = Vector3Length(delta);
                float diff = (distance - s.rest_length) / distance;

                if (!p1.fixed && !p2.fixed)
                {
                    p1.pos = p1.pos + delta * 0.5f * diff * s.stiffness * DT;
                    p2.pos = p2.pos - delta * 0.5f * diff * s.stiffness * DT;
                }
                else if (!p1.fixed)
                {
                    p1.pos = p1.pos + delta * diff * s.stiffness * DT;
                }
                else if (!p2.fixed)
                {
                    p2.pos = p2.pos - delta * diff * s.stiffness * DT;
                }
            }

            // Ground collision
            for (auto &p : particles)
            {
                if (p.pos.y < GROUND_Y)
                {
                    p.pos.y = GROUND_Y;
                    // Simple friction
                    p.pos.x = p.prev_pos.x + (p.pos.x - p.prev_pos.x) * 0.99f;
                    p.pos.z = p.prev_pos.z + (p.pos.z - p.prev_pos.z) * 0.99f;
                }
            }

            // Self-collisions
            handleSelfCollisions();
        }
    }

    void handleSelfCollisions()
    {
        const float particle_radius = particle_spacing * 0.5f;
        const float collision_distance = particle_radius * 2.0f;
        const float response_coef = 0.5f;

        for (size_t i = 0; i < particles.size(); i++)
        {
            for (size_t j = i + 1; j < particles.size(); j++)
            {
                if (particles[i].fixed && particles[j].fixed)
                    continue;

                Vector3 delta = particles[j].pos - particles[i].pos;
                float distance = Vector3Length(delta);

                if (distance < collision_distance && distance > 0.0001f)
                {
                    Vector3 direction = Vector3Normalize(delta);
                    float overlap = collision_distance - distance;

                    // Move particles apart
                    if (!particles[i].fixed && !particles[j].fixed)
                    {
                        particles[i].pos = particles[i].pos - direction * overlap * 0.5f * response_coef;
                        particles[j].pos = particles[j].pos + direction * overlap * 0.5f * response_coef;
                    }
                    else if (!particles[i].fixed)
                    {
                        particles[i].pos = particles[i].pos - direction * overlap * response_coef;
                    }
                    else if (!particles[j].fixed)
                    {
                        particles[j].pos = particles[j].pos + direction * overlap * response_coef;
                    }
                }
            }
        }
    }

    void handleBallInteraction()
    {
    }

    void handleMouseInteraction(Camera3D camera)
    {
        // Get mouse position in world space
        Vector2 mousePos = GetMousePosition();
        Ray mouseRay = GetMouseRay(mousePos, camera);

        // Find closest particle to mouse ray
        float closest_dist = FLT_MAX;
        int closest_idx = -1;

        for (size_t i = 0; i < particles.size(); i++)
        {
            if (particles[i].fixed)
                continue;

            // Simple distance from particle to ray
            Vector3 to_particle = particles[i].pos - mouseRay.position;
            float projection = Vector3DotProduct(to_particle, mouseRay.direction);
            Vector3 closest_point = mouseRay.position + Vector3Scale(mouseRay.direction, projection);
            float distance = Vector3Distance(particles[i].pos, closest_point);

            if (distance < closest_dist && distance < MOUSE_CUT_DISTANCE)
            {
                closest_dist = distance;
                closest_idx = i;
            }
        }

        // Clear previous selections
        for (auto &p : particles)
        {
            p.is_selected = false;
        }

        // Handle mouse input
        mouse_down = IsMouseButtonDown(MOUSE_LEFT_BUTTON);

        if (mouse_down && closest_idx != -1)
        {
            particles[closest_idx].is_selected = true;

            // Update mouse position in world space (projected to cloth plane)
            float t = -mouseRay.position.y / mouseRay.direction.y;
            mouse_pos = mouseRay.position + Vector3Scale(mouseRay.direction, t);

            // Apply mouse force to selected particle// Apply force to cloth
            Vector3 force = {
                GetMouseDelta().x * 0.1f,
                -GetMouseDelta().y * 0.1f,
                0};
            particles[closest_idx].acceleration = force;
            particles[closest_idx].pos = mouse_pos;
            particles[closest_idx].prev_pos = mouse_pos;
        }
    }

    void draw()
    {
        // Draw ground plane
        DrawPlane(Vector3{0, GROUND_Y, 0}, Vector2{100, 100}, BLACK);
        // Draw Grid Lines
        for (float x = -50; x <= 50; x += 1.0f)
        {
            DrawLine3D(Vector3{x, GROUND_Y, -50}, Vector3{x, GROUND_Y, 50}, LIGHTGRAY);
        }
        for (float z = -50; z <= 50; z += 1.0f)
        {
            DrawLine3D(Vector3{-50, GROUND_Y, z}, Vector3{50, GROUND_Y, z}, LIGHTGRAY);
        }

        // Draw springs
        for (auto &s : springs)
        {
            if (s.is_active)
            {
                DrawLine3D(particles[s.p1].pos, particles[s.p2].pos, BLUE);
            }
        }

        // Draw particles
        for (auto &p : particles)
        {
            if (p.is_selected)
            {
                DrawSphere(p.pos, particle_spacing * 0.15f, YELLOW);
            }
            else if (p.fixed)
            {
                DrawSphere(p.pos, particle_spacing * 0.1f, RED);
            }
            else
            {
                DrawSphere(p.pos, particle_spacing * 0.1f, GREEN);
            }
        }
    }
};

// Particle system
struct Particle
{
    Vector3 position;
    Vector3 velocity;
    Color color;
    float size;
    float lifetime;
    float lifeRemaining;
};

struct ParticleSystem
{
    std::vector<Particle> particles;
    Texture2D particleTexture;

    void Init()
    {
        // Create a simple white circle texture for particles
        Image img = GenImageColor(32, 32, BLANK);
        ImageDrawCircle(&img, 16, 16, 15, WHITE);
        particleTexture = LoadTextureFromImage(img);
        UnloadImage(img);
    }

    void AddParticle(Vector3 position, Vector3 velocity, Color color, float size, float lifetime)
    {
        if (particles.size() < MAX_PARTICLES)
        {
            Particle p;
            p.position = position;
            p.velocity = velocity;
            p.color = color;
            p.size = size;
            p.lifetime = lifetime;
            p.lifeRemaining = lifetime;
            particles.push_back(p);
        }
    }

    void Update()
    {
        for (auto it = particles.begin(); it != particles.end();)
        {
            it->position = it->position + it->velocity * GetFrameTime();
            it->velocity.y -= 0.1f * GetFrameTime(); // Gravity
            it->lifeRemaining -= GetFrameTime();

            if (it->lifeRemaining <= 0.0f)
            {
                it = particles.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    void Draw(Camera3D camera)
    {
        for (const auto &p : particles)
        {
            float lifeRatio = p.lifeRemaining / p.lifetime;
            Color drawColor = p.color;
            drawColor.a = (unsigned char)(lifeRatio * 255);

            DrawBillboard(
                camera,
                particleTexture,
                p.position,
                p.size * (0.5f + lifeRatio * 0.5f), // Shrink over time
                drawColor);
        }
    }

    void CreateExplosion(Vector3 position, int count, float power)
    {
        for (int i = 0; i < count; i++)
        {
            Vector3 vel = {
                (float)GetRandomValue(-100, 100) / 100.0f * power,
                (float)GetRandomValue(-100, 100) / 100.0f * power,
                (float)GetRandomValue(-100, 100) / 100.0f * power};

            Color color = {
                (unsigned char)GetRandomValue(200, 255),
                (unsigned char)GetRandomValue(100, 200),
                (unsigned char)GetRandomValue(0, 100),
                255};

            AddParticle(position, vel, color, 0.5f, PARTICLE_LIFETIME * (0.5f + GetRandomValue(0, 100) / 200.0f));
        }
    }

    void CreateRain(Vector3 area, int count)
    {
        for (int i = 0; i < count; i++)
        {
            Vector3 pos = {
                (float)GetRandomValue(-area.x / 2, area.x / 2),
                (float)GetRandomValue(area.y / 2, area.y),
                (float)GetRandomValue(-area.z / 2, area.z / 2)};

            Vector3 vel = {
                0,
                -GetRandomValue(50, 100) / 10.0f,
                0};

            AddParticle(pos, vel, BLUE, 0.1f, PARTICLE_LIFETIME * 2.0f);
        }
    }
};

// Main simulation class
struct PhysicsSandbox
{
    WaterSimulation water;
    Cloth cloth;
    ParticleSystem particles;
    Camera3D camera;
    bool isPaused;
    bool showGUI = false;

    PhysicsSandbox() : cloth(30, 30, 0.5f, false) {}

    void InitGUI()
    {
        InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, TITLE);
        SetTargetFPS(60);
        // GUI state variables
        showGUI = true;
    }

    void Init()
    {

        // Initialize camera
        camera.position = {10.0f, 10.0f, 10.0f};
        camera.target = {0.0f, 0.0f, 0.0f};
        camera.up = {0.0f, 1.0f, 0.0f};
        camera.fovy = 45.0f;
        camera.projection = CAMERA_PERSPECTIVE;

        // Initialize simulations
        water.Init();
        cloth = Cloth(20, 20, 0.5f, false);
        particles.Init();

        isPaused = false;
    }

    void Update()
    {
        if (IsKeyPressed(KEY_SPACE))
            isPaused = !isPaused;
        if (isPaused)
            return;

        // Update current simulation
        switch (currentMode)
        {
        case MODE_WATER:
            water.Update();
            break;
        case MODE_CLOTH:
            cloth.update();
            break;
        case MODE_PARTICLES:
            particles.Update();
            break;
        }

        // Handle interactions
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            Ray ray = GetMouseRay(GetMousePosition(), camera);

            switch (currentMode)
            {
            case MODE_WATER:
            {
                Vector3 planeNormal = {0, 1, 0};
                float planeD = -0.0f; // Plane equation: y = 0

                Vector3 rayDirection = ray.direction;
                Vector3 rayOrigin = ray.position;

                // Check if the ray intersects the plane
                float denom = Vector3DotProduct(planeNormal, rayDirection);
                if (fabs(denom) > 1e-6) // Avoid division by zero
                {
                    float t = -(Vector3DotProduct(planeNormal, rayOrigin) + planeD) / denom;
                    if (t >= 0)
                    {
                        Vector3 intersectionPoint = Vector3Add(rayOrigin, Vector3Scale(rayDirection, t));

                        // Check if the intersection point is within the bounds of the plane
                        if (intersectionPoint.x >= -10 && intersectionPoint.x <= 10 &&
                            intersectionPoint.z >= -10 && intersectionPoint.z <= 10)
                        {
                            water.AddSplash((int)((intersectionPoint.x + 10) / 20.0f * WATER_SIZE),
                                            (int)((intersectionPoint.z + 10) / 20.0f * WATER_SIZE),
                                            FORCE);
                        }
                    }
                }
            }
            break;

            case MODE_CLOTH:
            {

                cloth.handleMouseInteraction(camera);
            }
            break;

            case MODE_PARTICLES:
            {
                // Create particles at mouse position
                Vector3 pos = camera.target;
                particles.AddParticle(pos, {0, 0.5f, 0}, YELLOW, 0.3f, 1.0f);
            }
            break;
            }
        }

        // Keyboard controls
        if (IsKeyPressed(KEY_ONE))
            currentMode = MODE_WATER;
        if (IsKeyPressed(KEY_TWO))
            currentMode = MODE_CLOTH;
        if (IsKeyPressed(KEY_THREE))
            currentMode = MODE_PARTICLES;
        if (IsKeyPressed(KEY_E))
            particles.CreateExplosion(camera.target, 100, 2.0f);
        if (IsKeyPressed(KEY_R))
            particles.CreateRain({20, 20, 20}, 50);
    }

    void Draw()
    {
        BeginDrawing();
        ClearBackground(BLACK);

        BeginMode3D(camera);

        // Draw grid

        // Draw current simulation
        switch (currentMode)
        {
        case MODE_WATER:
            DrawGrid(20, 1.0f);
            water.Draw();
            break;
        case MODE_CLOTH:
            cloth.draw();
            break;
        case MODE_PARTICLES:
            DrawGrid(20, 1.0f);

            break; // Particles are drawn after 3D mode
        }

        // Always draw particles
        particles.Draw(camera);

        EndMode3D();

        // Draw UI
        DrawText(TextFormat("Mode: %s",
                            currentMode == MODE_WATER ? "Water" : currentMode == MODE_CLOTH ? "Cloth"
                                                                                            : "Particles"),
                 10, 10, 20, WHITE);
        DrawText(TextFormat("Particles: %d", particles.particles.size()), 10, 40, 20, WHITE);
        DrawText(isPaused ? "PAUSED" : "", 10, 70, 20, RED);

        DrawText("Controls:", 10, SCREEN_HEIGHT - 100, 20, WHITE);
        DrawText("1-3: Change mode", 10, SCREEN_HEIGHT - 80, 20, WHITE);
        DrawText("SPACE: Pause", 10, SCREEN_HEIGHT - 60, 20, WHITE);
        DrawText("E: Explosion", 10, SCREEN_HEIGHT - 40, 20, WHITE);
        DrawText("R: Rain", 10, SCREEN_HEIGHT - 20, 20, WHITE);

        if (showGUI)
        {
            // Water simulation parameters
            GuiLabel((Rectangle){150, 10, 200, 20}, "Water Simulation Parameters");
            GuiSlider((Rectangle){150, 70, 200, 20}, "WATER_DAMPING", TextFormat("%.3f", tempWaterDamping), &tempWaterDamping, 0.1f, 1.0f);
            WATER_DAMPING = tempWaterDamping;

            GuiSlider((Rectangle){150, 100, 200, 20}, "WATER_SPREAD", TextFormat("%.2f", tempWaterSpread), &tempWaterSpread, 0.1f, 1.0f);
            WATER_SPREAD = tempWaterSpread;

            GuiSlider((Rectangle){150, 130, 200, 20}, "WATER_HEIGHT", TextFormat("%.2f", tempWaterHeight), &tempWaterHeight, -10.0f, 10.0f);
            WATER_HEIGHT = tempWaterHeight;

            GuiSlider((Rectangle){150, 160, 200, 20}, "FORCE", TextFormat("%.2f", tempForce), &tempForce, 0.1f, 5.0f);
            FORCE = tempForce;

            // Particle parameters
            GuiLabel((Rectangle){150, 180, 200, 20}, "Particle Parameters");
            GuiSlider((Rectangle){150, 200, 200, 20}, "MAX_PARTICLES", TextFormat("%d", (int)tempMaxParticles), &tempMaxParticles, 100, 5000);
            MAX_PARTICLES = (int)tempMaxParticles;

            GuiSlider((Rectangle){150, 230, 200, 20}, "PARTICLE_LIFETIME", TextFormat("%.2f", tempParticleLifetime), &tempParticleLifetime, 1.0f, 10.0f);
            PARTICLE_LIFETIME = tempParticleLifetime;

            // General simulation parameters
            GuiLabel((Rectangle){150, 270, 200, 20}, "General Simulation Parameters");
            GuiSlider((Rectangle){150, 300, 200, 20}, "GRAVITY", TextFormat("%.2f", tempGravity), &tempGravity, -20.0f, 0.0f);
            GRAVITY = tempGravity;

            GuiSlider((Rectangle){150, 330, 200, 20}, "DT", TextFormat("%.3f", tempDT), &tempDT, 0.001f, 0.05f);
            DT = tempDT;

            GuiSlider((Rectangle){150, 360, 200, 20}, "SOLVER_ITERATIONS", TextFormat("%d", (int)tempSolverIterations), &tempSolverIterations, 1, 20);
            SOLVER_ITERATIONS = (int)tempSolverIterations;

            GuiSlider((Rectangle){150, 390, 200, 20}, "GROUND_Y", TextFormat("%.2f", tempGroundY), &tempGroundY, -20.0f, 0.0f);
            GROUND_Y = tempGroundY;

            GuiSlider((Rectangle){150, 420, 200, 20}, "MOUSE_INFLUENCE", TextFormat("%.2f", tempMouseInfluence), &tempMouseInfluence, 1.0f, 20.0f);
            MOUSE_INFLUENCE = tempMouseInfluence;

            GuiSlider((Rectangle){150, 450, 200, 20}, "MOUSE_CUT_DISTANCE", TextFormat("%.2f", tempMouseCutDistance), &tempMouseCutDistance, 1.0f, 20.0f);
            MOUSE_CUT_DISTANCE = tempMouseCutDistance;

            // Reset simulation button
            if (GuiButton((Rectangle){150, 490, 200, 30}, "Reset Simulation"))
            {
                Init();
            }
        }

        EndDrawing();
    }

    void Run()
    {
        while (!WindowShouldClose())
        {
            UpdateCamera(&camera, CAMERA_ORBITAL);
            Update();
            Draw();
        }

        CloseWindow();
    }
};

int main()
{
    PhysicsSandbox sandbox;
    sandbox.InitGUI();
    sandbox.Init();
    sandbox.Run();
    return 0;
}