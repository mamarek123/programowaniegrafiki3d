#version 410

in vec3 vertex_color; 

layout(location = 0) out vec4 vFragColor;

void main() {
    vFragColor = vec4(vertex_color, 1.0); 
}
