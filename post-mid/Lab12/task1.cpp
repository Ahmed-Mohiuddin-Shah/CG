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

void drawBezierCurve(Vector2 p0, Vector2 p1, Vector2 p2, int segments)
{
    for (int i = 0; i < segments; ++i)
    {
        float t1 = (float)i / (float)segments;
        float t2 = (float)(i + 1) / (float)segments;

        Vector2 point1 = QuadraticBezier(p0, p1, p2, t1);
        // Vector2 point2 = QuadraticBezier(p0, p1, p2, t2);

        DrawCircleV(point1, 2, BLUE);
    }
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

            Vector2 point1 = QuadraticBezier({p[0].x, p[0].y}, {p[1].x, p[1].y}, {p[2].x, p[2].y}, t1);

            DrawCircleV(point1, 2, BLUE);
        }
    }

};

struct ContinousCurve
{
    std::vector<Curve> curves;

    void addCurve(Curve c) {
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
    

    void draw()
    {
        for (Curve &c : curves)
        {
            c.draw();
            c.drawBezier(100);
        }
    }
    void drawBezier(int segments)
    {
        for (Curve &c : curves)
        {
            c.drawBezier(segments);
        }
    }
};

int main()
{
    InitWindow(800, 600, "Task 1 - Curve Visualization - Polyline vs Bezier");
    // SetTargetFPS(60);

    ContinousCurve continousCurve;

    // Create individual curves
    Curve curve1 = {{{100, 300}, {200, 100}, {300, 300}}};
    Curve curve2 = {{{300, 300}, {400, 500}, {500, 300}}};
    Curve curve3 = {{{500, 300}, {600, 100}, {700, 300}}};

    // Add curves to the continuous curve
    continousCurve.addCurve(curve1);
    continousCurve.addCurve(curve2);
    continousCurve.addCurve(curve3);

    while (!WindowShouldClose())
    {

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            for (int i = 0; i < continousCurve.curves.size(); i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    if (CheckCollisionPointCircle(GetMousePosition(), continousCurve.curves[i].p[j],40))
                    {
                        continousCurve.curves[i].p[j] = GetMousePosition();
                    }
                }
            }
        }

        // add a new curve
        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
        {
            Vector2 p0 = GetMousePosition();
            Vector2 p1 = {p0.x + 100, p0.y - 100};
            Vector2 p2 = {p0.x + 200, p0.y};

            Curve newCurve = {{p0, p1, p2}};
            continousCurve.addCurve(newCurve);
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw the continuous curve
        continousCurve.draw();

        EndDrawing();
    }

    CloseWindow();
}