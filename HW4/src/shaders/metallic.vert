#version 330 core

// TODO 5: 
// Implement Metallic shading

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;
out vec3 viewPos;

// Uniforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 cameraPos;

void main()
{
    // Transform the vertex position to world space
    FragPos = vec3(model * vec4(aPos, 1.0));

    // Transform to world space
    Normal = mat3(transpose(inverse(model))) * aNormal;

    TexCoord = aTexCoord;

    viewPos = cameraPos; // pass camera position to fragment shader

    gl_Position = projection * view * vec4(FragPos, 1.0);
}
