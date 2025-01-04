#version 330 core

// TODO 3:
// Implement Gouraud shading
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

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

out vec3 VertexColor;  
out vec2 TexCoord;

void main()
{
    // Transform position to world space
    vec4 worldPos = model * vec4(aPos, 1.0);
    vec3 FragPos = vec3(worldPos);

    // Transform and normalize normal
    vec3 N = normalize(mat3(transpose(inverse(model))) * aNormal);

    // Ambient
    vec3 ambient = light.ambient * material.ambient;

    // Diffuse
    vec3 L = normalize(light.position - FragPos);
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = light.diffuse * material.diffuse * diff;

    // Specular
    vec3 V = normalize(viewPos - FragPos);
    vec3 H = normalize(L + V);
    float spec = pow(max(dot(N, H), 0.0), material.gloss);
    vec3 specular = light.specular * material.specular * spec;

    // Result
    VertexColor = ambient + diffuse + specular;

    TexCoord = aTexCoord;
    gl_Position = projection * view * worldPos;
}
