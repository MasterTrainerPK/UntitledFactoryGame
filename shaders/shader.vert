#version 450

layout(push_constant, std430) uniform pc {
    mat4 data;
};

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;

layout(location = 0) out vec3 frag_color;

void main() {
    gl_Position = data * vec4(in_position, 1.0);
    frag_color = in_color;
}