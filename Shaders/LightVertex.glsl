#version 420 core

//This does not use the uniform buffer as every light has it's own view matrix.

layout(std140, binding = 1) uniform ProjView {
	mat4 projMatrix;
	mat4 viewMatrix;
} PV;

in vec3 position;

uniform float lightRadius;
uniform vec3 lightPos;
uniform vec4 lightColour;

void main(void) {
    vec3 scale = vec3(lightRadius);
    vec3 worldPos = (position * scale) + lightPos;
    gl_Position = (PV.projMatrix * PV.viewMatrix) * vec4(worldPos, 1.0);
}