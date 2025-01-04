#version 330 core
out vec4 FragColor;

in vec3 FragPos;   // Fragment position
in vec3 Normal;    // Normal vector
in vec2 TexCoord;  // Texture coordinate

uniform vec3 viewPos;
uniform samplerCube skybox;

uniform float AIR_coeff;
uniform float GLASS_coeff;
uniform float scale;
uniform float power;
uniform float bias;

// TODO 6-2
// Implement Glass-Empricial shading

void main() {
    // Normalize vectors
    vec3 norm = normalize(Normal);
    vec3 fromView = normalize(FragPos - viewPos);

    // Reflection vector
    vec3 reflectDir = fromView - 2.0f * dot(fromView, norm) * norm;

    // Refraction vector
    float eta = AIR_coeff / GLASS_coeff;
    vec3 refractDir;
    float IdotN = dot(fromView, norm);
    float K = 1.0f - eta * eta * (1.0f - IdotN * IdotN);
    if (K < 0) {
        refractDir = vec3(0.0f);
    } else {
        refractDir = eta * fromView - (eta * IdotN + sqrt(K)) * norm;
    }

    // Empirical approximation
    float R_theta = max(0.0f, min(1.0f, bias + scale * pow(1 + IdotN, power)));

    // Sample environment
    vec3 reflectColor = texture(skybox, reflectDir).rgb;
    vec3 refractColor = texture(skybox, refractDir).rgb;

    // Mix colors
    vec3 finalColor = R_theta * reflectColor + (1.0 - R_theta) * refractColor;

    FragColor = vec4(finalColor, 1.0);
}