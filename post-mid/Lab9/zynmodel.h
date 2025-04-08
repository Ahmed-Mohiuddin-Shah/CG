#ifndef ZYNMESH_H
#define ZYNMESH_H

#include "./helpers.h"
#include <glm/glm.hpp>
#include <vector>
#include <cstdio>
#include <string>
#include <glm/gtc/matrix_transform.hpp> // For glm::rotate

struct Triangle
{
    glm::vec3 v[3]; // Vertices of the triangle
    glm::vec2 t[3]; // Texture coordinates
    glm::vec3 n[3]; // Normals
};

struct ZMesh
{
    std::vector<Triangle> tris;

    bool loadFromObjectFile(const char *fileName)
    {
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> texCoords;
        std::vector<glm::vec3> normCoords;

        FILE *file = fopen(fileName, "r");
        if (!file)
        {
            printf("Failed to open file for reading\n");
            return false;
        }

        while (!feof(file))
        {
            char line[128];

            // Clear the line buffer
            for (int i = 0; i < sizeof(line); i++)
            {
                line[i] = '\0';
            }

            fgets(line, sizeof(line), file);
            if (line[0] == 'v')
            {
                if (line[1] == 't')
                {
                    glm::vec2 texCoord;
                    sscanf(line, "vt %f %f", &texCoord.x, &texCoord.y);
                    texCoords.push_back(texCoord);
                }
                else if (line[1] == 'n')
                {
                    glm::vec3 normCoord;
                    sscanf(line, "vn %f %f %f", &normCoord.x, &normCoord.y);
                    normCoords.push_back(normCoord);
                }
                else
                {
                    glm::vec3 vertex;
                    sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
                    vertices.push_back(vertex);
                }
            }
            else if (line[0] == 'f')
            {

                Triangle triangle;
                int v1, vt1, vn1, v2, vt2, vn2, v3, vt3, vn3;
                sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", &v1, &vt1, &vn1, &v2, &vt2, &vn2, &v3, &vt3, &vn3);

                triangle.v[0] = vertices[v1 - 1];
                triangle.v[1] = vertices[v2 - 1];
                triangle.v[2] = vertices[v3 - 1];
                // TODO Use texture resolution
                triangle.t[0] = ((texCoords[vt1 - 1]));
                triangle.t[1] = ((texCoords[vt2 - 1]));
                triangle.t[2] = ((texCoords[vt3 - 1]));
                triangle.n[0] = normCoords[vn1 - 1];
                triangle.n[1] = normCoords[vn2 - 1];
                triangle.n[2] = normCoords[vn3 - 1];

                tris.push_back(triangle);
            }
        }
        fclose(file);
        return true;
    }
};

struct ZModel
{

    ZMesh mesh;

    // Texture variables
    std::vector<GLuint> textureIDs;
    int texWidth, texHeight, texChannels;

    void loadModelTexture(const char *filename)
    {
        stbi_set_flip_vertically_on_load(true);
        // Load image data
        unsigned char *data = stbi_load(filename, &texWidth, &texHeight, &texChannels, 0);
        if (!data)
        {
            printf("Failed to load texture: %s\n", filename);
            exit(1);
        }

        // Create and bind texture
        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Upload texture data
        GLenum format = (texChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, texWidth, texHeight, 0, format, GL_UNSIGNED_BYTE, data);

        // Free image data
        stbi_image_free(data);

        // Store texture ID
        textureIDs.push_back(textureID);
    }

    bool loadModel(const char *modelName, const char *path, bool procedural = false)
    {

        std::string fullPath(path);

        if (procedural)
        {
            generateProceduralTexture(512, 512); // Generate a procedural texture
        }
        else
        {
            fullPath.append(path);
            fullPath.append(modelName);
            fullPath.append("_diffuse.png");
            loadModelTexture(fullPath.c_str()); // Load the texture
        }

        fullPath.clear();
        fullPath.append(path);
        fullPath.append(modelName);
        fullPath.append(".obj");
        if (!mesh.loadFromObjectFile(fullPath.c_str()))
        {
            return false;
        }
        return true;
    }

    void drawModel(int textureIndex = 0)
    {
        if (textureIndex < 0 || textureIndex >= textureIDs.size())
        {
            printf("Invalid texture index\n");
            return;
        }

        glBindTexture(GL_TEXTURE_2D, textureIDs[textureIndex]);
        glBegin(GL_TRIANGLES);

        for (const auto &tri : mesh.tris)
        {
            for (int i = 0; i < 3; i++)
            {
                glTexCoord2f(tri.t[i].x, tri.t[i].y);
                glNormal3f(tri.n[i].x, tri.n[i].y, tri.n[i].z);
                glVertex3f(tri.v[i].x, tri.v[i].y, tri.v[i].z);
            }
        }

        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void translateModel(const glm::vec3 &translation)
    {
        for (auto &tri : mesh.tris)
        {
            for (int i = 0; i < 3; i++)
            {
                tri.v[i] += translation;
            }
        }
    }

    void rotateModel(float angle, const glm::vec3 &axis)
    {
        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis);
        for (auto &tri : mesh.tris)
        {
            for (int i = 0; i < 3; i++)
            {
                glm::vec4 rotatedVertex = rotationMatrix * glm::vec4(tri.v[i], 1.0f);
                tri.v[i] = glm::vec3(rotatedVertex);
            }
        }
    }

    void scaleModel(const glm::vec3 &scaleFactors)
    {
        for (auto &tri : mesh.tris)
        {
            for (int i = 0; i < 3; i++)
            {
                tri.v[i] *= scaleFactors;
            }
        }
    }

    void generateProceduralTexture(int width, int height)
    {
        std::vector<unsigned char> textureData(width * height * 3);

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                int index = (y * width + x) * 3;

                // Checkerboard pattern
                bool isWhite = ((x / 32) % 2 == (y / 32) % 2);
                textureData[index] = isWhite ? 255 : 0;     // Red
                textureData[index + 1] = isWhite ? 255 : 0; // Green
                textureData[index + 2] = isWhite ? 255 : 0; // Blue
            }
        }

        // Create and bind texture
        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Upload texture data
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData.data());

        glBindTexture(GL_TEXTURE_2D, 0);

        // Store texture ID
        textureIDs.push_back(textureID);
    }
};

#endif