#version 430

out vec4 varyingColour;

layout (location=0) in vec3 position;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

void main(void)
{ 
    gl_Position = proj_matrix * mv_matrix * vec4(position, 1.0);
    varyingColour = vec4(position,1.0) * 0.5 + vec4(0.5, 0.5, 0.5, 0.5);
}