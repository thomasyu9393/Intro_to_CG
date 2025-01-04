#version 330 core
out vec4 FragColor;

in vec3 VertexColor; // Interpolated color from vertex shader
in vec2 TexCoord;    // Texture coordinate

uniform sampler2D ourTexture;

// TODO 3:
// Implement Gouraud shading

void main() {
    // Apply texture
    vec4 texColor = texture(ourTexture, TexCoord);
    FragColor = vec4(VertexColor, 1.0) * texColor;
}