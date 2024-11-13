#version 420

in vec3 vertex_color;

layout(location = 0) out vec4 vFragColor;

layout(std140, binding = 0) uniform Mixer {
    float strength;
    vec3 mix_color; 
};

void main() {
    vFragColor.rgb = mix(vertex_color, mix_color, strength); 
    vFragColor.a = 1.0; 
}
