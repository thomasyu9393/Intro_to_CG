#version 330 core
out vec4 FragColor;

in vec3 FragPos;   // Fragment position
in vec3 Normal;    // Normal vector
in vec2 TexCoord;  // Texture coordinate

uniform vec3 viewPos;
uniform samplerCube skybox;

uniform float AIR_coeff;
uniform float GLASS_coeff;

// TODO 6-1:
// Implement Glass-Schlick shading

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

    // Schlick's approximation
    float R0 = pow((AIR_coeff - GLASS_coeff) / (AIR_coeff + GLASS_coeff), 2.0);
    float R_theta = R0 + (1.0 - R0) * pow(1.0 + IdotN, 5.0);

    // Sample environment
    vec3 reflectColor = texture(skybox, reflectDir).rgb;
    vec3 refractColor = texture(skybox, refractDir).rgb;

    // Mix colors
    vec3 finalColor = R_theta * reflectColor + (1.0f - R_theta) * refractColor;

    FragColor = vec4(finalColor, 1.0);
}