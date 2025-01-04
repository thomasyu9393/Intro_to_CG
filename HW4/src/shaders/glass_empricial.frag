#version 330 core

// TODO 6-2
// Implement Glass-Empricial shading

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 viewPos;
uniform samplerCube skybox;
uniform float AIR_coeff;
uniform float GLASS_coeff;

uniform float scale;
uniform float power;
uniform float bias;

// Manual Refract Function
vec3 manualRefract(vec3 I, vec3 N, float eta) {
    float cosi = dot(I, N);
    float k = 1.0 - eta * eta * (1.0 - cosi * cosi);
    if (k < 0.0) {
        // Total internal reflection
        return vec3(0.0); // Alternatively, handle reflection here
    } else {
        return eta * I - (eta * cosi + sqrt(k)) * N;
    }
}

void main()
{
    // View direction
    vec3 V = normalize(viewPos - FragPos);
    vec3 N = normalize(Normal);

    // Compute reflection and refraction directions
    vec3 reflectDir = V - 2.0 * dot(N, V) * N;
    vec3 I = normalize(-V);
    vec3 refractDir = manualRefract(I, N, AIR_coeff / GLASS_coeff);
    // vec3 refractDir = refract(I, N, AIR_coeff / GLASS_coeff);
    


    // Sample environment
    vec3 reflectColor = texture(skybox, reflectDir).rgb;
    vec3 refractColor = texture(skybox, refractDir).rgb;

    // Empirical Approximation
    float R_theta = max(0.0, min(1.0, bias + scale * pow((1.0 + dot(I, N)), power)));
    // R_theta = 0.2 + 0.7 * (1 - cosTheta)^2

    // Mix reflection & refraction
    vec3 C_final = reflectColor * R_theta + refractColor * (1.0 - R_theta);

    FragColor = vec4(C_final, 1.0);
}
