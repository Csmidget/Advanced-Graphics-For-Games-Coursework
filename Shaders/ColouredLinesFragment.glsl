#version 330 core

uniform vec4 colour;

in Vertex {
	vec2 texCoord;
} IN;

out vec4 fragColour;

const int LINEDENSITY = 10;
const float LINETHICKNESS = 0.05;

void main(void) {
	vec2 scaledTexCoord = IN.texCoord * LINEDENSITY - vec2(0.5);
	float currOffsetX = abs(0.5 - fract(scaledTexCoord.x));
	float currOffsetY = abs(0.5 - fract(scaledTexCoord.y));

	float clampedX = clamp(currOffsetX,0.0,LINETHICKNESS);
	float clampedY = clamp(currOffsetY,0.0,LINETHICKNESS);

	float intensity = 0;
	intensity += pow(1 - (clampedX / LINETHICKNESS),2);	
	intensity += pow(1 - (clampedY / LINETHICKNESS),2);	

	fragColour = vec4(colour.xyz,intensity);
}