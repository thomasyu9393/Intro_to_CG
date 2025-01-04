#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

// TODO 4-1
// Implement CubeMap shading

void main() {
    FragColor = texture(skybox, TexCoords);
}