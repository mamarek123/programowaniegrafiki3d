#version 420

layout(std140, binding = 0) uniform Mixer {
    float strength;
    vec3 mix_color; 
};

in vec4 vertex_color;

out vec4 vFragColor;

void main() {
    vec3 mixed_color = mix(vertex_color.rgb, mix_color, strength);
    vFragColor = vec4(mixed_color, vertex_color.a);
}
