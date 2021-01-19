#version 410

layout(location=0) in  vec4 a_vertex_position;
layout(location=1) in vec2 a_vertex_color;
layout(location=2) in  vec3 a_vertex_normal;

out vec2 vertex_color;

layout(std140)  uniform Transformations {
    mat4 P;
    mat4 VM;
    mat3 N;
};

void main() {
    gl_Position = P * VM * a_vertex_position;
    vertex_color = a_vertex_color;
}
