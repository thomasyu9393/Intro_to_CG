#version 330 core

// TODO 4-1
// Implement CubeMap shading

layout(location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww; // Set w component to 1.0 to ensure depth is 1.0
}
