#version 330 core

// TODO 3:
// Implement Gouraud shading

in vec3 VertexColor; // Interpolated Gouraud color
in vec2 TexCoord;    // Interpolated texture coordinates

uniform sampler2D ourTexture;

out vec4 FragColor;

void main()
{
    vec3 texColor = texture(ourTexture, TexCoord).rgb;
    vec3 finalColor = VertexColor * texColor;

    FragColor = vec4(finalColor, 1.0);
}
