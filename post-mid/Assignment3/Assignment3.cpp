#include "raylib/raylib.h"
#include "raylib/raymath.h"
#include <vector>
#include <cmath>

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
    float gravity = 500.0f;
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

    void Update(float deltaTime) {
        for (auto& point : points) {
            if (!point.isPinned) {
                Vector2 velocity = Vector2Scale(Vector2Subtract(point.position, point.prevPosition), damping);
                point.prevPosition = point.position;
                point.position = Vector2Add(point.position, velocity);
                point.position.y += gravity * deltaTime;
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
};

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Cloth Simulator");
    SetTargetFPS(60);

    Cloth cloth(20, 15, 20, 200, 50);

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        cloth.Update(deltaTime);

        BeginDrawing();
        ClearBackground(BLACK);
        cloth.Draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}