#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 60) out;

in VS_OUT {
    vec2 texCoords;
} gs_in[];

out vec2 TexCoord;
uniform float time;
uniform bool exp;
uniform bool dup;

float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

vec4 explode(vec4 position, vec3 normal){
    float magnitude = 100.0;
    vec3 direction = normal * time * magnitude;
    return position + vec4(direction, 0.0);
}

vec3 GetNormal(){
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
    return normalize(cross(a, b));
}

void main() {
    vec3 normal = GetNormal();
    if (dup) {
        int num = 2;
        for (int numX = -num; numX <= num; ++numX) {
            for (int numY = -num; numY <= num; ++numY) {
                for (int numZ = -num; numZ <= num; ++numZ) {
                    if (numX == 0 && numY == 0) {
                        continue;
                    }
                    vec3 offset = vec3(1.0f * numX, 1.0f * numY, 1.0f * numZ); // Grid offset
                    offset = normalize(offset);
                    for (int i = 0; i < 3; ++i) {
                        gl_Position = explode(gl_in[i].gl_Position, offset);
                        TexCoord = gs_in[i].texCoords;
                        EmitVertex();
                    }
                    EndPrimitive();
                }
            }
        }
    } else {
        for(int i = 0; i < 3; ++i){
            if (exp) {
                gl_Position = explode(gl_in[i].gl_Position, normal);
            } else {
                gl_Position = gl_in[i].gl_Position;
            }
            TexCoord = gs_in[i].texCoords;
            EmitVertex();
        }
        EndPrimitive();
    }
}
