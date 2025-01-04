#version 330 core
out vec4 FragColor;

in vec3 FragPos;   // Fragment position
in vec3 Normal;    // Normal vector
in vec2 TexCoord;  // Texture coordinate

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float gloss;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

uniform sampler2D ourTexture;

// TODO 2
// Implement Bling-Phong shading

void main() {
    // Ambient
    vec3 ambient = light.ambient * material.ambient;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.gloss);
    vec3 specular = light.specular * (spec * material.specular);

    // Combine results
    vec3 lighting = ambient + diffuse + specular;

    // Apply texture
    vec4 texColor = texture(ourTexture, TexCoord);
    FragColor = vec4(lighting, 1.0) * texColor;
}