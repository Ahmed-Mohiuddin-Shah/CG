#version 330 core

in vec3 FragPos;                         // Fragment position in world space
in vec3 Normal;                          // Normal in world space

out vec4 FragColor;                      // Output color

uniform vec3 lightPos;                   // Light position
uniform vec3 lightColor;                 // Light color
uniform vec3 viewPos;                    // Camera position
uniform vec3 matAmbient;                 // Material ambient color
uniform vec3 matDiffuse;                 // Material diffuse color
uniform vec3 matSpecular;                // Material specular color
uniform float shininess;                 // Material shininess

void main() {
    // Ambient lighting
    vec3 ambient = matAmbient * lightColor;

    // Diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * matDiffuse * lightColor;

    // Specular lighting
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = spec * matSpecular * lightColor;

    // Combine results
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}