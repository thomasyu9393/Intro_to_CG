#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 view;
uniform mat4 projection;

// TODO 4-1
// Implement CubeMap shading

void main() {
    // Pass the direction vector (the vertex position is the direction for the skybox)
    TexCoords = aPos;

    // Transform the position into clip space
    gl_Position = projection * mat4(mat3(view)) * vec4(aPos, 1.0);
    gl_Position = gl_Position.xyww; // Keep depth fixed for the skybox
}