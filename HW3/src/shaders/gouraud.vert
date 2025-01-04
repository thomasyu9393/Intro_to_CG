#version 330 core
layout (location = 0) in vec3 aPos;      // Vertex position
layout (location = 1) in vec3 aNormal;   // Vertex normal
layout (location = 2) in vec2 aTexCoord; // Texture coordinate

out vec3 VertexColor; // Color to be interpolated
out vec2 TexCoord;    // Texture coordinate

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

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// TODO 3:
// Implement Gouraud shading

void main() {
    // Transform vertex position
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    // Calculate world-space position and normal
    vec3 FragPos = vec3(model * vec4(aPos, 1.0));
    vec3 Normal = mat3(transpose(inverse(model))) * aNormal;

    // Ambient
    vec3 ambient = light.ambient * material.ambient;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);

    // Specular
    vec3 toView = normalize(viewPos - FragPos);
    vec3 reflectDir = -lightDir - 2 * dot(-lightDir, norm) * norm;
    float spec = pow(max(dot(toView, reflectDir), 0.0), material.gloss);
    vec3 specular = light.specular * (spec * material.specular);

    // Combine results
    VertexColor = ambient + diffuse + specular;

    // Pass texture coordinates
    TexCoord = aTexCoord;
}