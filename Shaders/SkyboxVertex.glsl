#version 420 core
uniform mat4 modelMatrix;
in vec3 position;

layout(std140, binding = 1) uniform ProjView {
	mat4 projMatrix;
	mat4 viewMatrix;
} PV;

out Vertex {
    vec3 viewDir;
} OUT;

void main(void) {
    vec3 pos = position;
    mat4 invproj = inverse(PV.projMatrix);
    pos.xy *= vec2(invproj[0][0], invproj[1][1]);
    pos.z = -1.0f;
    OUT.viewDir = transpose(mat3(PV.viewMatrix)) * normalize(pos);
    gl_Position = vec4(position, 1.0);
}


