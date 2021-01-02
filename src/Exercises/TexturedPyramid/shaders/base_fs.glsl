#version 410

layout(location=0) out vec4 vFragColor;
in vec2 vertex_color;

uniform sampler2D diffuse_map;

layout(std140) uniform Modifiers {
    float strength;
    vec3 light;
};

void main() {

    vFragColor = texture(diffuse_map, vertex_color);
}
