#version 330 core
out vec4 FragColor;

in vec3 FragPos;   // Fragment position
in vec3 Normal;    // Normal vector
in vec2 TexCoord;  // Texture coordinate

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;
uniform vec3 viewPos;
uniform float bias;
uniform float alpha;
uniform float lightIntensity;

uniform sampler2D ourTexture;
uniform samplerCube skybox;

// TODO 5:
// Implement Metallic shading

void main() {
    // Normalize vectors
    vec3 norm = normalize(Normal);
    vec3 fromView = normalize(FragPos - viewPos);
    vec3 toLight = normalize(light.position - FragPos);

    // Lambertian reflection (diffuse)
    float NdotL = max(dot(norm, toLight), 0.0);
    float B_d = NdotL * lightIntensity;

    // Texture color
    vec3 modelColor = texture(ourTexture, TexCoord).rgb;

    // Reflection vector
    vec3 reflectDir = fromView - 2 * dot(fromView, norm) * norm;
    vec3 reflectColor = texture(skybox, reflectDir).rgb;

    // Mix the colors
    vec3 finalColor = alpha * (B_d + bias) * modelColor + (1.0 - alpha) * reflectColor;

    FragColor = vec4(finalColor, 1.0);
}