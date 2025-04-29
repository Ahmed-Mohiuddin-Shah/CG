#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <algorithm>
#include <list>
#include <string>
#include <fstream>
#include <strstream>

using namespace std;

// Texture coordinate structure
struct vec2d {
    float u = 0;
    float v = 0;
    float w = 1;
};

// 3D vector structure
struct vec3d {
    float x = 0;
    float y = 0;
    float z = 0;
    float w = 1;
};

// Triangle structure with texture coordinates
struct triangle {
    vec3d p[3];
    vec2d t[3];
    Color col;
};

// Mesh structure
struct mesh {
    vector<triangle> tris;
    bool LoadFromObjectFile(string sFilename, bool bHasTexture = false)
    {
        ifstream f(sFilename);
        if (!f.is_open())
            return false;

        // Local cache of verts
        vector<vec3d> verts;
        vector<vec2d> texs;

        while (!f.eof())
        {
            char line[128];
            f.getline(line, 128);

            strstream s;
            s << line;

            char junk;

            if (line[0] == 'v')
            {
                if (line[1] == 't')
                {
                    vec2d v;
                    s >> junk >> junk >> v.u >> v.v;
                    texs.push_back(v);
                }
                else
                {
                    vec3d v;
                    s >> junk >> v.x >> v.y >> v.z;
                    verts.push_back(v);
                }
            }

            if (!bHasTexture)
            {
                if (line[0] == 'f')
                {
                    int f[3];
                    s >> junk >> f[0] >> f[1] >> f[2];
                    tris.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
                }
            }
            else
            {
                if (line[0] == 'f')
                {
                    s >> junk;

                    string tokens[6];
                    int nTokenCount = -1;

                    while (!s.eof())
                    {
                        char c = s.get();
                        if (c == ' ' || c == '/')
                            nTokenCount++;
                        else
                            tokens[nTokenCount].append(1, c);
                    }

                    tokens[nTokenCount].pop_back();

                    tris.push_back({ verts[stoi(tokens[0]) - 1], verts[stoi(tokens[2]) - 1], verts[stoi(tokens[4]) - 1],
                        texs[stoi(tokens[1]) - 1], texs[stoi(tokens[3]) - 1], texs[stoi(tokens[5]) - 1] });
                }
            }
        }
        return true;
    }
};

// 4x4 matrix structure
struct mat4x4 {
    float m[4][4] = { 0 };
};

// Camera structure
struct camera {
    vec3d pos;
    vec3d lookDir;
    float yaw;
    float pitch;
    float moveSpeed;
    float rotSpeed;
};

// Global variables
mesh meshCube;
Texture2D texBrick;
Image img;
camera cam;
float fNear = 0.1f;
float fFar = 1000.0f;
float fFov = 90.0f;
float fAspectRatio;
float fFovRad;

// Function declarations
void MultiplyMatrixVector(vec3d& i, vec3d& o, mat4x4& m);
mat4x4 Matrix_MakeIdentity();
mat4x4 Matrix_MakeRotationX(float fAngleRad);
mat4x4 Matrix_MakeRotationY(float fAngleRad);
mat4x4 Matrix_MakeRotationZ(float fAngleRad);
mat4x4 Matrix_MakeTranslation(float x, float y, float z);
mat4x4 Matrix_MakeProjection();
mat4x4 Matrix_MultiplyMatrix(mat4x4& m1, mat4x4& m2);
mat4x4 Matrix_PointAt(vec3d& pos, vec3d& target, vec3d& up);
mat4x4 Matrix_QuickInverse(mat4x4& m);
vec3d Matrix_MultiplyVector(mat4x4& m, vec3d& i);
vec3d Vector_Add(vec3d& v1, vec3d& v2);
vec3d Vector_Sub(vec3d& v1, vec3d& v2);
vec3d Vector_Mul(vec3d& v1, float k);
vec3d Vector_Div(vec3d& v1, float k);
float Vector_DotProduct(vec3d& v1, vec3d& v2);
float Vector_Length(vec3d& v);
vec3d Vector_Normalise(vec3d& v);
vec3d Vector_CrossProduct(vec3d& v1, vec3d& v2);
void DrawTexturedTriangle(int x1, int y1, float u1, float v1, float w1,
    int x2, int y2, float u2, float v2, float w2,
    int x3, int y3, float u3, float v3, float w3,
    Image tex);

int main()
{
    // Initialize window
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Software Renderer");

    // Initialize camera
    cam.pos = { 0, 0, -5 };
    cam.lookDir = { 0, 0, 1 };
    cam.yaw = 0;
    cam.pitch = 0;
    cam.moveSpeed = 5.0f;
    cam.rotSpeed = 2.0f;

    // Load texture
    texBrick = LoadTexture("brmarble.jpg");
    img = LoadImageFromTexture(texBrick);

    // Load cube mesh
    meshCube.LoadFromObjectFile("cube.obj", true);

    // Calculate aspect ratio and FOV
    fAspectRatio = (float)screenHeight / (float)screenWidth;
    fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * PI);

    // Main game loop
    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        // Handle input
        float deltaTime = GetFrameTime();

        // Rotation
        if (IsKeyDown(KEY_LEFT)) cam.yaw -= cam.rotSpeed * deltaTime;
        if (IsKeyDown(KEY_RIGHT)) cam.yaw += cam.rotSpeed * deltaTime;
        if (IsKeyDown(KEY_UP)) cam.pitch += cam.rotSpeed * deltaTime;
        if (IsKeyDown(KEY_DOWN)) cam.pitch -= cam.rotSpeed * deltaTime;

        // Movement
        vec3d vForward = Vector_Mul(cam.lookDir, cam.moveSpeed * deltaTime);

        if (IsKeyDown(KEY_W)) cam.pos = Vector_Add(cam.pos, vForward);
        if (IsKeyDown(KEY_S)) cam.pos = Vector_Sub(cam.pos, vForward);

        if (IsKeyDown(KEY_A))
        {
            vec3d vLeft = { -vForward.z, 0, vForward.x };
            cam.pos = Vector_Add(cam.pos, vLeft);
        }

        if (IsKeyDown(KEY_D))
        {
            vec3d vRight = { vForward.z, 0, -vForward.x };
            cam.pos = Vector_Add(cam.pos, vRight);
        }

        // Create rotation matrices
        mat4x4 matRotZ, matRotX;
        matRotZ = Matrix_MakeRotationZ(cam.yaw);
        matRotX = Matrix_MakeRotationX(cam.pitch);

        // Combine rotations
        mat4x4 matRot = Matrix_MultiplyMatrix(matRotX, matRotZ);

        // Calculate look direction
        vec3d vTarget = { 0, 0, 1 };
        cam.lookDir = Matrix_MultiplyVector(matRot, vTarget);
        cam.lookDir = Vector_Normalise(cam.lookDir);

        // Create camera matrix
        vec3d vUp = { 0, 1, 0 };
        vec3d vTargetPos = Vector_Add(cam.pos, cam.lookDir);
        mat4x4 matCamera = Matrix_PointAt(cam.pos, vTargetPos, vUp);
        mat4x4 matView = Matrix_QuickInverse(matCamera);

        // Create projection matrix
        mat4x4 matProj = Matrix_MakeProjection();

        // Store triangles for sorting
        vector<triangle> vecTrianglesToRaster;

        // Draw triangles
        BeginDrawing();
        ClearBackground(BLACK);

        // Process each triangle in the mesh
        for (auto tri : meshCube.tris)
        {
            triangle triTransformed, triViewed, triProjected;

            // Transform triangle by rotation and translation
            triTransformed.p[0] = Matrix_MultiplyVector(matRot, tri.p[0]);
            triTransformed.p[1] = Matrix_MultiplyVector(matRot, tri.p[1]);
            triTransformed.p[2] = Matrix_MultiplyVector(matRot, tri.p[2]);

            // Offset into the scene
            triTransformed.p[0].z += 3.0f;
            triTransformed.p[1].z += 3.0f;
            triTransformed.p[2].z += 3.0f;

            // Copy texture coordinates
            triTransformed.t[0] = tri.t[0];
            triTransformed.t[1] = tri.t[1];
            triTransformed.t[2] = tri.t[2];

            // Calculate triangle normal
            vec3d normal, line1, line2;
            line1 = Vector_Sub(triTransformed.p[1], triTransformed.p[0]);
            line2 = Vector_Sub(triTransformed.p[2], triTransformed.p[0]);
            normal = Vector_CrossProduct(line1, line2);
            normal = Vector_Normalise(normal);

            // Cull triangles that are facing away from the camera
            vec3d vCameraRay = Vector_Sub(triTransformed.p[0], cam.pos);
            if (Vector_DotProduct(normal, vCameraRay) < 0.0f)
            {
                // Convert world space to view space
                triViewed.p[0] = Matrix_MultiplyVector(matView, triTransformed.p[0]);
                triViewed.p[1] = Matrix_MultiplyVector(matView, triTransformed.p[1]);
                triViewed.p[2] = Matrix_MultiplyVector(matView, triTransformed.p[2]);

                // Copy texture coordinates
                triViewed.t[0] = triTransformed.t[0];
                triViewed.t[1] = triTransformed.t[1];
                triViewed.t[2] = triTransformed.t[2];

                // Project triangles from 3D to 2D
                triProjected.p[0] = Matrix_MultiplyVector(matProj, triViewed.p[0]);
                triProjected.p[1] = Matrix_MultiplyVector(matProj, triViewed.p[1]);
                triProjected.p[2] = Matrix_MultiplyVector(matProj, triViewed.p[2]);

                // Copy texture coordinates
                triProjected.t[0] = triViewed.t[0];
                triProjected.t[1] = triViewed.t[1];
                triProjected.t[2] = triViewed.t[2];

                // Scale into view
                triProjected.p[0].x += 1.0f; triProjected.p[0].y += 1.0f;
                triProjected.p[1].x += 1.0f; triProjected.p[1].y += 1.0f;
                triProjected.p[2].x += 1.0f; triProjected.p[2].y += 1.0f;

                triProjected.p[0].x *= 0.5f * (float)screenWidth;
                triProjected.p[0].y *= 0.5f * (float)screenHeight;
                triProjected.p[1].x *= 0.5f * (float)screenWidth;
                triProjected.p[1].y *= 0.5f * (float)screenHeight;
                triProjected.p[2].x *= 0.5f * (float)screenWidth;
                triProjected.p[2].y *= 0.5f * (float)screenHeight;

                // Store triangle for sorting
                vecTrianglesToRaster.push_back(triProjected);
            }
        }

        // Sort triangles from back to front
        sort(vecTrianglesToRaster.begin(), vecTrianglesToRaster.end(), [](triangle& t1, triangle& t2)
            {
                float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
                float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
                return z1 > z2;
            });

        // Draw the triangles
        for (auto& triToRaster : vecTrianglesToRaster)
        {
            // Draw textured triangle
            DrawTexturedTriangle(
                triToRaster.p[0].x, triToRaster.p[0].y, triToRaster.t[0].u, triToRaster.t[0].v, triToRaster.p[0].w,
                triToRaster.p[1].x, triToRaster.p[1].y, triToRaster.t[1].u, triToRaster.t[1].v, triToRaster.p[1].w,
                triToRaster.p[2].x, triToRaster.p[2].y, triToRaster.t[2].u, triToRaster.t[2].v, triToRaster.p[2].w,
                img
            );

            // Draw triangle wireframe
            DrawTriangleLines(
                { triToRaster.p[0].x, triToRaster.p[0].y },
                { triToRaster.p[1].x, triToRaster.p[1].y },
                { triToRaster.p[2].x, triToRaster.p[2].y },
                WHITE
            );
        }

        // Draw FPS
        DrawFPS(10, 10);
        EndDrawing();
    }

    // Cleanup
    UnloadTexture(texBrick);
    CloseWindow();

    return 0;
}

// Matrix multiplication with vector
vec3d Matrix_MultiplyVector(mat4x4& m, vec3d& i)
{
    vec3d v;
    v.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + i.w * m.m[3][0];
    v.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + i.w * m.m[3][1];
    v.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + i.w * m.m[3][2];
    v.w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + i.w * m.m[3][3];
    return v;
}

// Matrix multiplication
mat4x4 Matrix_MultiplyMatrix(mat4x4& m1, mat4x4& m2)
{
    mat4x4 matrix;
    for (int c = 0; c < 4; c++)
        for (int r = 0; r < 4; r++)
            matrix.m[r][c] = m1.m[r][0] * m2.m[0][c] + m1.m[r][1] * m2.m[1][c] + m1.m[r][2] * m2.m[2][c] + m1.m[r][3] * m2.m[3][c];
    return matrix;
}

// Identity matrix
mat4x4 Matrix_MakeIdentity()
{
    mat4x4 matrix;
    matrix.m[0][0] = 1.0f;
    matrix.m[1][1] = 1.0f;
    matrix.m[2][2] = 1.0f;
    matrix.m[3][3] = 1.0f;
    return matrix;
}

// Rotation X matrix
mat4x4 Matrix_MakeRotationX(float fAngleRad)
{
    mat4x4 matrix;
    matrix.m[0][0] = 1.0f;
    matrix.m[1][1] = cosf(fAngleRad);
    matrix.m[1][2] = sinf(fAngleRad);
    matrix.m[2][1] = -sinf(fAngleRad);
    matrix.m[2][2] = cosf(fAngleRad);
    matrix.m[3][3] = 1.0f;
    return matrix;
}

// Rotation Y matrix
mat4x4 Matrix_MakeRotationY(float fAngleRad)
{
    mat4x4 matrix;
    matrix.m[0][0] = cosf(fAngleRad);
    matrix.m[0][2] = sinf(fAngleRad);
    matrix.m[2][0] = -sinf(fAngleRad);
    matrix.m[1][1] = 1.0f;
    matrix.m[2][2] = cosf(fAngleRad);
    matrix.m[3][3] = 1.0f;
    return matrix;
}

// Rotation Z matrix
mat4x4 Matrix_MakeRotationZ(float fAngleRad)
{
    mat4x4 matrix;
    matrix.m[0][0] = cosf(fAngleRad);
    matrix.m[0][1] = sinf(fAngleRad);
    matrix.m[1][0] = -sinf(fAngleRad);
    matrix.m[1][1] = cosf(fAngleRad);
    matrix.m[2][2] = 1.0f;
    matrix.m[3][3] = 1.0f;
    return matrix;
}

// Translation matrix
mat4x4 Matrix_MakeTranslation(float x, float y, float z)
{
    mat4x4 matrix;
    matrix.m[0][0] = 1.0f;
    matrix.m[1][1] = 1.0f;
    matrix.m[2][2] = 1.0f;
    matrix.m[3][3] = 1.0f;
    matrix.m[3][0] = x;
    matrix.m[3][1] = y;
    matrix.m[3][2] = z;
    return matrix;
}

// Projection matrix
mat4x4 Matrix_MakeProjection()
{
    mat4x4 matrix;
    matrix.m[0][0] = fAspectRatio * fFovRad;
    matrix.m[1][1] = fFovRad;
    matrix.m[2][2] = fFar / (fFar - fNear);
    matrix.m[3][2] = (-fFar * fNear) / (fFar - fNear);
    matrix.m[2][3] = 1.0f;
    matrix.m[3][3] = 0.0f;
    return matrix;
}

// Point at matrix
mat4x4 Matrix_PointAt(vec3d& pos, vec3d& target, vec3d& up)
{
    // Calculate new forward direction
    vec3d newForward = Vector_Sub(target, pos);
    newForward = Vector_Normalise(newForward);

    // Calculate new Up direction
    vec3d a = Vector_Mul(newForward, Vector_DotProduct(up, newForward));
    vec3d newUp = Vector_Sub(up, a);
    newUp = Vector_Normalise(newUp);

    // New Right direction is cross product
    vec3d newRight = Vector_CrossProduct(newUp, newForward);

    // Construct dimensioning and translation matrix
    mat4x4 matrix;
    matrix.m[0][0] = newRight.x;    matrix.m[0][1] = newRight.y;    matrix.m[0][2] = newRight.z;    matrix.m[0][3] = 0.0f;
    matrix.m[1][0] = newUp.x;        matrix.m[1][1] = newUp.y;        matrix.m[1][2] = newUp.z;        matrix.m[1][3] = 0.0f;
    matrix.m[2][0] = newForward.x;    matrix.m[2][1] = newForward.y;    matrix.m[2][2] = newForward.z;    matrix.m[2][3] = 0.0f;
    matrix.m[3][0] = pos.x;            matrix.m[3][1] = pos.y;            matrix.m[3][2] = pos.z;            matrix.m[3][3] = 1.0f;
    return matrix;
}

// Quick matrix inverse (only for rotation/translation matrices)
mat4x4 Matrix_QuickInverse(mat4x4& m)
{
    mat4x4 matrix;
    matrix.m[0][0] = m.m[0][0]; matrix.m[0][1] = m.m[1][0]; matrix.m[0][2] = m.m[2][0]; matrix.m[0][3] = 0.0f;
    matrix.m[1][0] = m.m[0][1]; matrix.m[1][1] = m.m[1][1]; matrix.m[1][2] = m.m[2][1]; matrix.m[1][3] = 0.0f;
    matrix.m[2][0] = m.m[0][2]; matrix.m[2][1] = m.m[1][2]; matrix.m[2][2] = m.m[2][2]; matrix.m[2][3] = 0.0f;
    matrix.m[3][0] = -(m.m[3][0] * matrix.m[0][0] + m.m[3][1] * matrix.m[1][0] + m.m[3][2] * matrix.m[2][0]);
    matrix.m[3][1] = -(m.m[3][0] * matrix.m[0][1] + m.m[3][1] * matrix.m[1][1] + m.m[3][2] * matrix.m[2][1]);
    matrix.m[3][2] = -(m.m[3][0] * matrix.m[0][2] + m.m[3][1] * matrix.m[1][2] + m.m[3][2] * matrix.m[2][2]);
    matrix.m[3][3] = 1.0f;
    return matrix;
}

// Vector addition
vec3d Vector_Add(vec3d& v1, vec3d& v2)
{
    return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
}

// Vector subtraction
vec3d Vector_Sub(vec3d& v1, vec3d& v2)
{
    return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
}

// Vector multiplication with scalar
vec3d Vector_Mul(vec3d& v1, float k)
{
    return { v1.x * k, v1.y * k, v1.z * k };
}

// Vector division by scalar
vec3d Vector_Div(vec3d& v1, float k)
{
    return { v1.x / k, v1.y / k, v1.z / k };
}

// Dot product
float Vector_DotProduct(vec3d& v1, vec3d& v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

// Vector length
float Vector_Length(vec3d& v)
{
    return sqrtf(Vector_DotProduct(v, v));
}

// Normalize vector
vec3d Vector_Normalise(vec3d& v)
{
    float l = Vector_Length(v);
    return { v.x / l, v.y / l, v.z / l };
}

// Cross product
vec3d Vector_CrossProduct(vec3d& v1, vec3d& v2)
{
    vec3d v;
    v.x = v1.y * v2.z - v1.z * v2.y;
    v.y = v1.z * v2.x - v1.x * v2.z;
    v.z = v1.x * v2.y - v1.y * v2.x;
    return v;
}

// Draw textured triangle
void DrawTexturedTriangle(int x1, int y1, float u1, float v1, float w1,
    int x2, int y2, float u2, float v2, float w2,
    int x3, int y3, float u3, float v3, float w3,
    Image tex)
{
    // Sort the points in order of ascending y
    if (y2 < y1) {
        swap(y1, y2); swap(x1, x2); swap(u1, u2); swap(v1, v2); swap(w1, w2);
    }
    if (y3 < y1) {
        swap(y1, y3); swap(x1, x3); swap(u1, u3); swap(v1, v3); swap(w1, w3);
    }
    if (y3 < y2) {
        swap(y2, y3); swap(x2, x3); swap(u2, u3); swap(v2, v3); swap(w2, w3);
    }

    int dy1 = y2 - y1;
    int dx1 = x2 - x1;
    float du1 = u2 - u1;
    float dv1 = v2 - v1;
    float dw1 = w2 - w1;

    int dy2 = y3 - y1;
    int dx2 = x3 - x1;
    float du2 = u3 - u1;
    float dv2 = v3 - v1;
    float dw2 = w3 - w1;

    float dax_step = 0, dbx_step = 0;
    float du1_step = 0, dv1_step = 0, dw1_step = 0;
    float du2_step = 0, dv2_step = 0, dw2_step = 0;

    if (dy1) dax_step = dx1 / (float)abs(dy1);
    if (dy2) dbx_step = dx2 / (float)abs(dy2);

    if (dy1) du1_step = du1 / (float)abs(dy1);
    if (dy1) dv1_step = dv1 / (float)abs(dy1);
    if (dy1) dw1_step = dw1 / (float)abs(dy1);

    if (dy2) du2_step = du2 / (float)abs(dy2);
    if (dy2) dv2_step = dv2 / (float)abs(dy2);
    if (dy2) dw2_step = dw2 / (float)abs(dy2);

    // Draw the first part of the triangle (top to middle)
    if (dy1)
    {
        for (int i = y1; i <= y2; i++)
        {
            int ax = x1 + (i - y1) * dax_step;
            int bx = x1 + (i - y1) * dbx_step;

            float tex_su = u1 + (i - y1) * du1_step;
            float tex_sv = v1 + (i - y1) * dv1_step;
            float tex_sw = w1 + (i - y1) * dw1_step;

            float tex_eu = u1 + (i - y1) * du2_step;
            float tex_ev = v1 + (i - y1) * dv2_step;
            float tex_ew = w1 + (i - y1) * dw2_step;

            if (ax > bx)
            {
                swap(ax, bx);
                swap(tex_su, tex_eu);
                swap(tex_sv, tex_ev);
                swap(tex_sw, tex_ew);
            }

            float tstep = 1.0f / ((float)(bx - ax));
            float t = 0.0f;

            for (int j = ax; j < bx; j++)
            {
                float tu = (1.0f - t) * tex_su + t * tex_eu;
                float tv = (1.0f - t) * tex_sv + t * tex_ev;
                float tw = (1.0f - t) * tex_sw + t * tex_ew;

                if (tw > 0)
                {
                    Color color = GetImageColor(tex, (int)((tu / tw) * tex.width) % tex.width, (int)((tv / tw) * tex.height) % tex.height);
                    DrawPixel(j, i, color);
                }

                t += tstep;
            }
        }
    }

    // Prepare for second part of the triangle
    dy1 = y3 - y2;
    dx1 = x3 - x2;
    du1 = u3 - u2;
    dv1 = v3 - v2;
    dw1 = w3 - w2;

    if (dy1) dax_step = dx1 / (float)abs(dy1);
    if (dy2) dbx_step = dx2 / (float)abs(dy2);

    du1_step = 0, dv1_step = 0, dw1_step = 0;
    if (dy1) du1_step = du1 / (float)abs(dy1);
    if (dy1) dv1_step = dv1 / (float)abs(dy1);
    if (dy1) dw1_step = dw1 / (float)abs(dy1);

    // Draw the second part of the triangle (middle to bottom)
    if (dy1)
    {
        for (int i = y2; i <= y3; i++)
        {
            int ax = x2 + (i - y2) * dax_step;
            int bx = x1 + (i - y1) * dbx_step;

            float tex_su = u2 + (i - y2) * du1_step;
            float tex_sv = v2 + (i - y2) * dv1_step;
            float tex_sw = w2 + (i - y2) * dw1_step;

            float tex_eu = u1 + (i - y1) * du2_step;
            float tex_ev = v1 + (i - y1) * dv2_step;
            float tex_ew = w1 + (i - y1) * dw2_step;

            if (ax > bx)
            {
                swap(ax, bx);
                swap(tex_su, tex_eu);
                swap(tex_sv, tex_ev);
                swap(tex_sw, tex_ew);
            }

            float tstep = 1.0f / ((float)(bx - ax));
            float t = 0.0f;

            for (int j = ax; j < bx; j++)
            {
                float tu = (1.0f - t) * tex_su + t * tex_eu;
                float tv = (1.0f - t) * tex_sv + t * tex_ev;
                float tw = (1.0f - t) * tex_sw + t * tex_ew;

                if (tw > 0)
                {
                    Color color = GetImageColor(tex, (int)((tu / tw) * tex.width) % tex.width, (int)((tv / tw) * tex.height) % tex.height);
                    DrawPixel(j, i, color);
                }

                t += tstep;
            }
        }
    }
}