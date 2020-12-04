#version 420 core

uniform mat4 modelMatrix;
uniform mat4 textureMatrix;

layout(std140, binding = 1) uniform ProjView {
	mat4 projMatrix;
	mat4 viewMatrix;
} PV;

in vec3 position;
in vec3 normal;
in vec2 texCoord;

out Vertex {
    vec3 colour;
    vec2 texCoord;
    vec3 normal;
    vec3 worldPos;
} OUT;

void main(void) {
    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));

    OUT.texCoord = (textureMatrix * vec4(texCoord, 0.0, 1.0)).xy;
    OUT.normal = normalize(normalMatrix * normalize(normal));
    vec4 worldPos = (modelMatrix * vec4(position,1));
    OUT.worldPos = worldPos.xyz;
    gl_Position = (PV.projMatrix * PV.viewMatrix) * worldPos;
}