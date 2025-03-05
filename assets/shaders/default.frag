#version 330 core

in vec2 uvs;

uniform vec4 color;
uniform sampler2D texture_id;

out vec4 Frag_Color;

void main() {
    Frag_Color = texture(texture_id, uvs) * color;
}