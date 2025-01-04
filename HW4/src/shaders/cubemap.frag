#version 330 core

// TODO 4-1
// Implement CubeMap shading

in vec3 TexCoords;

out vec4 FragColor;

uniform samplerCube skybox; // Cubemap sampler

void main()
{    
    FragColor = texture(skybox, TexCoords);
}
