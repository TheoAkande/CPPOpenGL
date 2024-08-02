#version 430

uniform mat4 shadowMVP;

out vec4 color;

void main(void) {
    color = vec4(0.0, 0.0, 1.0, 1.0);
}