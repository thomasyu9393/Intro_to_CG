#version 330 core
out vec4 FragColor;

in vec2 TexCoord; 

uniform sampler2D ourTexture;
uniform vec3 rainbowColor;

void main() {
    // Implement Rainbow Effect
    vec4 texColor = texture(ourTexture, TexCoord);
    if (rainbowColor != vec3(1.0, 1.0, 1.0)) {
        FragColor = vec4(texColor.rgb * rainbowColor, texColor.a);
    } else {
        FragColor = texColor;
    }
}