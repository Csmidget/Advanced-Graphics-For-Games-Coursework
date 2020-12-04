#version 420 core
uniform mat4 modelMatrix;
uniform mat4 textureMatrix;

layout(std140, binding = 1) uniform ProjView {
	mat4 projMatrix;
	mat4 viewMatrix;
} PV;


in vec3 position;
in vec4 colour;
in vec3 normal;
in vec4 tangent;
in vec2 texCoord;

out Vertex {
    vec4 colour;
    vec2 texCoord;
    vec3 normal;
} OUT;

void main(void) {
    OUT.colour = colour;
    OUT.texCoord = (textureMatrix * vec4(texCoord, 0.0, 1.0)).xy;

    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));

    OUT.normal = normalize(normalMatrix * normalize(normal));

    gl_Position = (PV.projMatrix * PV.viewMatrix) * modelMatrix * vec4(position, 1.0);
}