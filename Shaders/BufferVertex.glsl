#version 330 core
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

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
    OUT.texCoord = texCoord;

    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));

    OUT.normal = normalize(normalMatrix * normalize(normal));

    gl_Position = (projMatrix * viewMatrix) * modelMatrix * vec4(position, 1.0);
}