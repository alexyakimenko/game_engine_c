#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_uvs;

uniform mat4 projection;
uniform mat4 model;

out vec2 uvs;

void main() {
    uvs = a_uvs;
    gl_Position = projection * model * vec4(a_pos, 1.0);
}