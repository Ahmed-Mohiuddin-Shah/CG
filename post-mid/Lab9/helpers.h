#ifndef HELPERS_H
#define HELPERS_H

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void loadTexture(const char *filename, GLuint &textureID, int &texWidth, int &texHeight, int &texChannels)
{
    // Load image data
    unsigned char *data = stbi_load(filename, &texWidth, &texHeight, &texChannels, 0);
    if (!data)
    {
        printf("Failed to load texture: %s\n", filename);
        exit(1);
    }

    // Create and bind texture
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
}

#endif