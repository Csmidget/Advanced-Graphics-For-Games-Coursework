#version 420 core

uniform mat4 modelMatrix;
uniform mat4 textureMatrix;

layout(std140, binding = 1) uniform ProjView {
	mat4 projMatrix;
	mat4 viewMatrix;
} PV;

in vec3 position;
in vec2 texCoord;

out Vertex {
	vec2 texCoord;
} OUT;

void main(void) {
	mat4 mvp = PV.projMatrix * PV.viewMatrix * modelMatrix;
	gl_Position = mvp * vec4(position, 1.0);
	OUT.texCoord = (textureMatrix * vec4(texCoord, 0.0, 1.0)).xy;
}