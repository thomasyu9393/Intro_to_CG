#version 330 core
layout (location = 0) in vec3 aPos;      // Vertex position
layout (location = 1) in vec3 aNormal;   // Vertex normal
layout (location = 2) in vec2 aTexCoord; // Texture coordinate

out vec3 FragPos;   // Fragment position
out vec3 Normal;    // Normal vector
out vec2 TexCoord;  // Texture coordinate

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// TODO 6-2
// Implement Glass-Empricial shading

void main() {
    // Transform vertex position
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    // Calculate world-space position and normal
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;

    // Pass texture coordinates
    TexCoord = aTexCoord;
}