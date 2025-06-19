#version 450

layout(location = 0) in vec4 in_position;

layout(location = 0) out vec3 frag_color;

vec3 colors[3] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);

void main() {
    gl_Position = in_position;
    frag_color = colors[gl_VertexIndex];
}