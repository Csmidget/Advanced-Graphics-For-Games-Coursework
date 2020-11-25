#version 330 core

uniform float offset;

uniform sampler2D baseTex;
uniform sampler2D circuitTex;
uniform sampler2D overlayTex;

in Vertex {
	vec2 texCoord;
} IN;

out vec4 fragColour;

//Sin wave modifiers
const float frequency = 0.08;
const float amplitude = 0.5;
const float verticalOffset = 1;

void main(void) {

	fragColour = texture(baseTex, IN.texCoord);

    float distFromCentre = length(IN.texCoord - vec2(0.5,0.5));

    //Handy tool for calculating wave modifiers: https://www.desmos.com/calculator/w9jrdpvsmk
    float overlayStrength = amplitude * sin( (distFromCentre + offset) / frequency ) + verticalOffset; 

    fragColour += texture(circuitTex, IN.texCoord) * texture(overlayTex, IN.texCoord) * overlayStrength;
}

