#version 150 core

uniform sampler2D diffuseTex;
uniform sampler2D noiseTex;

in Vertex {
	vec2 texCoord;
} IN;

out vec4 fragColour;

void main(void) {
	fragColour = texture(diffuseTex, IN.texCoord);
    fragColour = fragColour * texture(noiseTex, IN.texCoord);
}