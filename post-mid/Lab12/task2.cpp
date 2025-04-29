#include <raylib.h>
#include <cmath>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

int main()
{
    InitWindow(1280, 720, "Task 2 - Curve Visualization - Parametric Curves");
    SetTargetFPS(60);

    float t = 0.0f;                // Parameter for animation
    float spiralT = 0.0f;           // Parameter for spiral
    float circleradius = 100.0f;   // Radius for circle
    float ellipseRadiusX = 100.0f; // X radius for ellipse
    float ellipseRadiusY = 50.0f;  // Y radius for ellipse
    float angleIncrement = 0.01f;  // Increment for animation
    int sign = 1;             // Sign for direction
    float spiralSpacing = 5.0f;    // Spacing for spiral
    float length = 4.0f;           // Length of spiral

    while (!WindowShouldClose())
    {
        // Update
        t += angleIncrement;
        spiralT += angleIncrement * sign;

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw Circle
        for (float theta = 0; theta < 2 * PI; theta += 0.01f)
        {
            float x = 400 + circleradius * cos(theta);
            float y = 300 + circleradius * sin(theta);
            DrawPixel(x, y, RED);
        }

        // Draw Ellipse
        for (float theta = 0; theta < 2 * PI; theta += 0.01f)
        {
            float x = 600 + ellipseRadiusX * cos(theta);
            float y = 400 + (ellipseRadiusY / 2) * sin(theta);
            DrawPixel(x, y, BLUE);
        }

        // Draw Spiral
        for (float theta = 0; theta < length * PI; theta += 0.01f)
        {
            float x = 400 + (spiralSpacing * theta) * cos(theta);
            float y = 300 + (spiralSpacing * theta) * sin(theta);
            DrawPixel(x, y, GREEN);
        }

        // Animate a point along the circle
        float pointX = 400 + circleradius * cos(t);
        float pointY = 300 + circleradius * sin(t);
        DrawCircle(pointX, pointY, 5, BLACK);

        // Animate a point along the ellipse
        float ellipseX = 600 + ellipseRadiusX * cos(t);
        float ellipseY = 400 + (ellipseRadiusY / 2) * sin(t);
        DrawCircle(ellipseX, ellipseY, 5, BLACK);

        // Animate a point along the spiral according to length

        if (spiralT > length * PI)
        {
            // spiralT = 0.0f;
            sign *= -1; // Reverse direction
        }
        else if (spiralT < 0)
        {
            // spiralT = 0.0f;
            sign *= -1; // Reverse direction
        }

        float spiralX = 400 + (spiralSpacing * spiralT) * cos(spiralT);
        float spiralY = 300 + (spiralSpacing * spiralT) * sin(spiralT);
        DrawCircle(spiralX, spiralY, 5, BLACK);

        // Draw UI
        GuiLabel((Rectangle){150, 10, 200, 20}, "Circle Animation");

        // Sliders for Circle
        GuiSlider((Rectangle){150, 40, 200, 20}, "Circle Radius", NULL, &circleradius, 50.0f, 200.0f);

        // Sliders for Ellipse
        GuiSlider((Rectangle){150, 70, 200, 20}, "Ellipse Radius X", NULL, &ellipseRadiusX, 50.0f, 200.0f);
        GuiSlider((Rectangle){150, 100, 200, 20}, "Ellipse Radius Y", NULL, &ellipseRadiusY, 25.0f, 100.0f);

        // Sliders for Spiral
        GuiSlider((Rectangle){150, 130, 200, 20}, "Spiral Spacing", NULL, &spiralSpacing, 1.0f, 20.0f);
        GuiSlider((Rectangle){150, 160, 200, 20}, "Spiral Length", NULL, &length, 1.0f, 20.0f);

        GuiSlider((Rectangle){150, 190, 200, 20}, "Angle Increment", NULL, &angleIncrement, 0.01f, 0.1f);
        GuiLabel((Rectangle){150, 220, 200, 20}, "Press ESC to exit");

        // printf("Circle Radius: %.2f\n", spiralT);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}