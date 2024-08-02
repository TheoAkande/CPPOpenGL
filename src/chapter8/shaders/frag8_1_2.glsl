#version 430

in vec3 varyingNormal, varyingLightDir, varyingVertPos, varyingHalfVec;
in vec4 shadow_coord;

out vec4 fragColor;

struct PositionalLight
{
    vec4 ambient, diffuse, specular;
    vec3 position;
};
struct Material
{
    vec4 ambient, diffuse, specular;
    float shininess;
};

uniform vec4 globalAmbient;
uniform PositionalLight light;
uniform Material material;
uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
uniform mat4 norm_matrix;
uniform mat4 shadowMVP2;

layout (binding = 0) uniform sampler2DShadow shTex;

void main(void)
{
    // vec3 L = normalize(varyingLightDir);
    // vec3 N = normalize(varyingNormal);
    // vec3 E = normalize(-varyingVertPos);
    // vec3 H = normalize(varyingHalfVec);

    // float notInShadow = textureProj(shTex, shadow_coord);

    // fragColor = globalAmbient * material.ambient + light.ambient * material.ambient;
    // if (notInShadow == 1.0) {
    //     fragColor += light.diffuse * material.diffuse * max(dot(L, N), 0.0)
    //     + light.specular * material.specular * pow(max(dot(H, N), 0.0), material.shininess * 3.0);
    // }
    fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}