#version 410

layout(location=0) out vec4 vFragColor;
in vec3 vertex_color;

layout(std140) uniform Modifiers {
    float strength;
    vec3 light;
};

void main() {
    vFragColor.a = 1.0;
    vFragColor.rgb = strength*light*vertex_color.rgb;
}
