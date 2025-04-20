#include "raylib/raylib.h"
#include "raylib/raymath.h"
#include <vector>
#include <cmath>
#include <cstdlib> // For rand()

struct Point {
    Vector2 position;
    Vector2 prevPosition;
    bool isPinned;

    Point(float x, float y, bool pinned = false) : position({x, y}), prevPosition({x, y}), isPinned(pinned) {}
};

struct Stick {
    Point* p1;
    Point* p2;
    float length;

    Stick(Point* a, Point* b) : p1(a), p2(b), length(Vector2Distance(a->position, b->position)) {}
};

class Cloth {
public:
    std::vector<Point> points;
    std::vector<Stick> sticks;
    float gravity = 9.81f;
    float damping = 0.99f;

    Cloth(int width, int height, int spacing, int startX, int startY) {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                bool pinned = (y == 0 && (x % 5 == 0)); // Pin every 5th point on the top row
                points.emplace_back(startX + x * spacing, startY + y * spacing, pinned);
            }
        }

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                if (x < width - 1) sticks.emplace_back(&points[y * width + x], &points[y * width + x + 1]);
                if (y < height - 1) sticks.emplace_back(&points[y * width + x], &points[(y + 1) * width + x]);
            }
        }
    }

    void Update(float deltaTime, Vector2 wind) {
        for (auto& point : points) {
            if (!point.isPinned) {
                Vector2 velocity = Vector2Scale(Vector2Subtract(point.position, point.prevPosition), damping);
                point.prevPosition = point.position;
                point.position = Vector2Add(point.position, velocity);
                point.position.y += gravity * deltaTime;

                // Apply wind force with random fluttering
                Vector2 flutter = {((rand() % 100) / 100.0f - 0.5f) * 10.0f, ((rand() % 100) / 100.0f - 0.5f) * 10.0f};
                point.position = Vector2Add(point.position, Vector2Scale(Vector2Add(wind, flutter), deltaTime));
            }
        }

        for (int i = 0; i < 5; i++) { // Multiple iterations for stability
            for (auto& stick : sticks) {
                Vector2 delta = Vector2Subtract(stick.p2->position, stick.p1->position);
                float dist = Vector2Length(delta);
                float diff = (dist - stick.length) / dist;
                Vector2 offset = Vector2Scale(delta, diff * 0.5f);

                if (!stick.p1->isPinned) stick.p1->position = Vector2Add(stick.p1->position, offset);
                if (!stick.p2->isPinned) stick.p2->position = Vector2Subtract(stick.p2->position, offset);
            }
        }
    }

    void Draw() {
        for (auto& stick : sticks) {
            DrawLineV(stick.p1->position, stick.p2->position, WHITE);
        }
        for (auto& point : points) {
            DrawCircleV(point.position, 3, point.isPinned ? RED : BLUE);
        }
    }

    void TogglePinPoint(Vector2 mousePosition) {
        for (auto& point : points) {
            if (Vector2Distance(point.position, mousePosition) < 5.0f) { // Check if mouse is near the point
                point.isPinned = !point.isPinned; // Toggle pin state
                break;
            }
        }
    }
};

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Cloth Simulator with Wind");
    SetTargetFPS(60);

    Cloth cloth(20, 15, 20, 200, 50);

    Vector2 wind = {100.0f, 0.0f}; // Initial wind direction
    float windAngle = 0.0f;        // Angle of the wind in radians
    float windSpeed = 100.0f;      // Wind speed

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        // Rotate wind direction using A and D keys
        if (IsKeyDown(KEY_A)) windAngle -= 1.0f * deltaTime;
        if (IsKeyDown(KEY_D)) windAngle += 1.0f * deltaTime;

        // Adjust wind speed using W and S keys
        if (IsKeyDown(KEY_W)) windSpeed += 50.0f * deltaTime;
        if (IsKeyDown(KEY_S)) windSpeed -= 50.0f * deltaTime;
        windSpeed = Clamp(windSpeed, 0.0f, 500.0f); // Clamp wind speed to a reasonable range

        // Add randomness of 1-2% to wind speed
        float randomFactor = 1.0f + ((rand() % 3 - 1) * 0.01f); // Random factor between 0.99 and 1.02
        windSpeed *= randomFactor;

        // Update wind vector based on angle and speed
        wind = {cosf(windAngle) * windSpeed, sinf(windAngle) * windSpeed};

        // Toggle pin state on mouse click
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mousePosition = GetMousePosition();
            cloth.TogglePinPoint(mousePosition);
        }

        cloth.Update(deltaTime, wind);

        BeginDrawing();
        ClearBackground(BLACK);

        cloth.Draw();

        // Draw wind direction arrow
        Vector2 arrowStart = {50, 50};
        Vector2 arrowEnd = Vector2Add(arrowStart, Vector2Scale(wind, 0.5f));
        DrawLineEx(arrowStart, arrowEnd, 3, YELLOW);
        DrawCircleV(arrowEnd, 5, YELLOW);

        // Draw streak effects for wind visualization
        for (int i = 0; i < 10; i++) {
            Vector2 streakStart = {50 + i * 10.0f, 100};
            Vector2 streakEnd = Vector2Add(streakStart, Vector2Scale(wind, 0.1f));
            DrawLineEx(streakStart, streakEnd, 2, Fade(WHITE, 0.5f));
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}