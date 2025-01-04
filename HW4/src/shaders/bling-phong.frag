#version 330 core

// TODO 2
// Implement Bling-Phong shading

// structs for Light and Material
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

// uniform
uniform Material material;
uniform Light light;
uniform vec3 viewPos;

uniform sampler2D ourTexture;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

void main()
{
    // Ambient Lighting
    vec3 ambient = light.ambient * material.ambient * texture(ourTexture, TexCoord).rgb;

    // Diffuse Lighting
    vec3 N = normalize(Normal);     // Normal
    vec3 L = normalize(light.position - FragPos);   // Light direction
    float diff = max(dot(L, N), 0.0);       // Compute L ᐧ N (make sure it's not negative)
    vec3 diffuse = light.diffuse * (diff * material.diffuse * texture(ourTexture, TexCoord).rgb);

    // Specular Lighting
    vec3 V = normalize(viewPos - FragPos);      // View direction
    vec3 H = normalize(L + V);                  // Halfway vector
    float spec = pow(max(dot(N, H), 0.0), material.gloss);  // specular factor
    vec3 specular = light.specular * material.specular * spec;

    // Combine
    vec3 result = ambient + diffuse + specular;
    
    FragColor = vec4(result, 1.0);
}

