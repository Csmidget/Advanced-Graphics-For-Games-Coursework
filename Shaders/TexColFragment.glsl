#version 150 core

uniform sampler2D diffuseTex;

in Vertex {
	vec2 texCoord;
	vec4 colour;
} IN;

out vec4 fragColour;

void main(void) {
	fragColour = IN.colour * texture(diffuseTex, IN.texCoord);
}