#version 330 core

// TODO 5:
// Implement Metallic shading

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec3 viewPos;

out vec4 FragColor;

// Uniforms
uniform sampler2D ourTexture; 
uniform samplerCube skybox;       
uniform vec3 lightPos;          
uniform float bias;        
uniform float alpha;      
uniform float lightIntensity; 

void main()
{
    // Normalize vectors
    vec3 N = normalize(Normal);
    vec3 V = normalize(viewPos - FragPos);    // View direction
    vec3 L = normalize(lightPos - FragPos);   // Light direction

    // Lambertian diffuse component
    float Bd = max(dot(L, N) * lightIntensity, 0.0);

    // Add bias
    float B = Bd + bias;

    // Sample the model's texture color
    vec3 modelColor = texture(ourTexture, TexCoord).rgb;

    // Compute reflection direction
    // reflect(I, N)
    vec3 reflectDir = V - 2.0 * dot(N, V) * N;

    // Sample the environment color from the cubemap
    vec3 reflectColor = texture(skybox, reflectDir).rgb;

    // Mix model color and reflect color
    // C_final = alpha * B * C_model_color + (1 - alpha)*C_reflect
    vec3 C_final = (alpha * B * modelColor) + ((1.0 - alpha) * reflectColor);

    FragColor = vec4(C_final, 1.0);
}
