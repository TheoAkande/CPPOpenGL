#version 430

layout (location=0) in vec3 pos;
layout (location=1) in vec2 texCoord;

out vec4 varyingColour;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

layout (binding=0) uniform sampler2D samp; // not used in vertex shader

void main(void)
{	 
    gl_Position = proj_matrix * mv_matrix * vec4(pos,1.0);
    varyingColour = vec4(pos, 1.0) * 0.5 + vec4(0.5, 0.5, 0.5, 0.5);
}