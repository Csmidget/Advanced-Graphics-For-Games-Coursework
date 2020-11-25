#version 330 core

uniform sampler2D sceneTex;

in Vertex {
    vec2 texCoord;
} IN;

out vec4 fragColor;

float doubleStrength = 0.5f;
vec2 doubleOffset = vec2(0.05f,0);

void main(void) {

  //  vec2 delta = dFdy(IN.texCoord) + dFdx(IN.texCoord);

    vec2 range = vec2(1 - doubleOffset);

    vec4 normalCol = texture(sceneTex, IN.texCoord.xy * range);
    vec4 doubleCol = texture(sceneTex, IN.texCoord.xy * range + doubleOffset);

    fragColor = doubleStrength * doubleCol + (1 - doubleStrength) * normalCol;
}