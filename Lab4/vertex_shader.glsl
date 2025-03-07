#version 330 core

layout(location = 0) in vec3 aPos;       // Vertex position
layout(location = 1) in vec3 aNormal;    // Vertex normal

out vec3 FragPos;                        // Fragment position in world space
out vec3 Normal;                         // Normal in world space

uniform mat4 model;                      // Model matrix
uniform mat4 view;                       // View matrix
uniform mat4 projection;                 // Projection matrix

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0)); // Transform vertex to world space
    Normal = mat3(transpose(inverse(model))) * aNormal; // Transform normal to world space

    gl_Position = projection * view * model * vec4(aPos, 1.0); // Final vertex position
}