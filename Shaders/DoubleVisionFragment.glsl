#version 330 core

uniform sampler2D sceneTex;
uniform float doubleXOffset;

in Vertex {
    vec2 texCoord;
} IN;

out vec4 fragColor;

float doubleStrength = 0.5f;

void main(void) {

    vec2 doubleOffset = vec2(doubleXOffset,0.0f);  

    vec2 range = vec2(1 - doubleOffset);

    vec4 normalCol = texture(sceneTex, IN.texCoord.xy * range);
    vec4 doubleCol = texture(sceneTex, IN.texCoord.xy * range + doubleOffset);

    fragColor = doubleStrength * doubleCol + (1 - doubleStrength) * normalCol;
}