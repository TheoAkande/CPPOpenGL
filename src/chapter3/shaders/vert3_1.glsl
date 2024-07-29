#version 430
uniform float offset;
uniform float rotation;

// builds and returns a translation matrix
mat4 buildTranslate(float x, float y, float z)
{ 
    mat4 trans = mat4(1.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    x, y, z, 1.0 );
    return trans;
}

// builds and returns a matrix that performs a rotation around the Z axis
mat4 buildRotateZ(float rad)
{ 
    mat4 zrot = mat4(cos(rad), -sin(rad), 0.0, 0.0,
    sin(rad), cos(rad), 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 1.0 );
    return zrot;
}

void main(void)
{ 
    mat4 position = mat4(
        0.25, -0.25, 0.25, 0.0,
        -0.25, -0.25, 0.25, 0.0,
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    );

    position = position * buildTranslate(offset, 0.0, 0.0) * buildRotateZ(rotation);

    gl_Position = vec4(position[0][gl_VertexID], position[1][gl_VertexID], position[2][gl_VertexID], 1.0);
}