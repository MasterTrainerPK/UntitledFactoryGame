#version 450

layout(location = 0) in vec4 in_position;
layout(location = 1) in vec3 in_color;

layout(location = 0) out vec3 frag_color;

void main() {
    gl_Position = vec4(in_position[0] / in_position[2], in_position[1] / in_position[2], in_position[2] / 5.0, in_position[3]);
    frag_color = in_color;
}