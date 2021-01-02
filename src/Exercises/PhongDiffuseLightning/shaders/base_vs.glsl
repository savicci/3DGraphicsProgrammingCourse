#version 410

layout(location=0) in  vec4 a_vertex_position;
layout(location=1) in vec2 a_vertex_color;

out vec2 vertex_color;

uniform PVM {
    mat4 proj_matrix;
};

void main() {
    gl_Position = proj_matrix * a_vertex_position;
    vertex_color = a_vertex_color;
}
