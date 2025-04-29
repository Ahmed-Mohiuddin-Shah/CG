#include <raylib.h>
#include <cmath>
#include <vector>
#include <algorithm>

// Function to calculate a point on a quadratic Bezier curve
Vector2 QuadraticBezier(Vector2 p0, Vector2 p1, Vector2 p2, float t)
{
    float u = 1.0f - t;
    float tt = t * t;
    float uu = u * u;

    Vector2 point = {
        uu * p0.x + 2 * u * t * p1.x + tt * p2.x,
        uu * p0.y + 2 * u * t * p1.y + tt * p2.y};

    return point;
}

struct Curve
{
    Vector2 p[3];

    void draw()
    {
        DrawLine(p[0].x, p[0].y, p[1].x, p[1].y, BLUE);
        DrawLine(p[1].x, p[1].y, p[2].x, p[2].y, BLUE);
        DrawCircle(p[0].x, p[0].y, 5, RED);
        DrawCircle(p[1].x, p[1].y, 5, RED);
        DrawCircle(p[2].x, p[2].y, 5, RED);
    }
    void drawBezier(int segments)
    {
        for (int i = 0; i < segments; ++i)
        {
            float t1 = (float)i / (float)segments;

            Vector2 point1 = QuadraticBezier(p[0], p[1], p[2], t1);

            DrawCircleV(point1, 2, BLUE);
        }
    }
};

struct ContinousCurve
{
    std::vector<Curve> curves;

    void addCurve(Curve c)
    {
        if (curves.empty())
        {
            curves.push_back(c);
        }
        else
        {
            Curve &lastCurve = curves.back();
            lastCurve.p[2] = c.p[0]; // Connect the end of the last curve to the start of the new curve
            curves.push_back(c);
        }
    }

    void enforceC1Continuity()
    {
        for (size_t i = 1; i < curves.size(); ++i)
        {
            curves[i].p[0] = curves[i - 1].p[2]; // Shared control point
            curves[i].p[1] = {
                2 * curves[i].p[0].x - curves[i - 1].p[1].x,
                2 * curves[i].p[0].y - curves[i - 1].p[1].y}; // Adjust tangent
        }
    }

    void enforceG1Continuity()
    {
        for (size_t i = 1; i < curves.size(); ++i)
        {
            curves[i].p[0] = curves[i - 1].p[2]; // Shared control point
            Vector2 tangent = {
                curves[i].p[0].x - curves[i - 1].p[1].x,
                curves[i].p[0].y - curves[i - 1].p[1].y}; // Tangent direction
            float length = sqrt(tangent.x * tangent.x + tangent.y * tangent.y);
            tangent.x /= length;
            tangent.y /= length;
            float magnitude = sqrt(pow(curves[i].p[1].x - curves[i].p[0].x, 2) +
                                   pow(curves[i].p[1].y - curves[i].p[0].y, 2));
            curves[i].p[1] = {
                curves[i].p[0].x + tangent.x * magnitude,
                curves[i].p[0].y + tangent.y * magnitude}; // Adjust tangent direction
        }
    }

    void draw()
    {
        for (Curve &c : curves)
        {
            c.draw();
            c.drawBezier(100);
        }
    }
};

int main()
{
    InitWindow(800, 600, "Task 3 - Curve Visualization - G1 vs C1 Continuity");
    SetTargetFPS(60);

    ContinousCurve continousCurve;

    // Create individual curves
    Curve curve1 = {{{100, 300}, {200, 100}, {300, 300}}};
    Curve curve2 = {{{300, 300}, {400, 500}, {500, 300}}};
    Curve curve3 = {{{500, 300}, {600, 100}, {700, 300}}};

    // Add curves to the continuous curve
    continousCurve.addCurve(curve1);
    continousCurve.addCurve(curve2);
    continousCurve.addCurve(curve3);

    bool enforceC1 = false;
    bool enforceG1 = false;

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_ONE))
        {
            enforceC1 = true;
            enforceG1 = false;
            continousCurve.enforceC1Continuity();
        }
        if (IsKeyPressed(KEY_TWO))
        {
            enforceC1 = false;
            enforceG1 = true;
            continousCurve.enforceG1Continuity();
        }

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            for (int i = 0; i < continousCurve.curves.size(); i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    if (CheckCollisionPointCircle(GetMousePosition(), continousCurve.curves[i].p[j], 40))
                    {
                        continousCurve.curves[i].p[j] = GetMousePosition();
                    }
                }
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Press 1 for C1 Continuity", 10, 10, 20, DARKGRAY);
        DrawText("Press 2 for G1 Continuity", 10, 40, 20, DARKGRAY);

        continousCurve.draw();

        EndDrawing();
    }

    CloseWindow();
}