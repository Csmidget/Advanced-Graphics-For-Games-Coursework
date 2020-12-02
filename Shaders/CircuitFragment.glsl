#version 330 core

uniform float offset;

uniform sampler2D baseTex;
uniform sampler2D circuitTex;
uniform sampler2D overlayTex;
uniform sampler2D normalTex;

in Vertex {
    vec4 colour;
    vec2 texCoord;
    vec3 normal;
    vec3 tangent;
    vec3 binormal;
    vec3 worldPos;
} IN;

out vec4 fragColour[2];


//Sin wave modifiers
const float frequency = 0.08;
const float amplitude = 0.5;
const float verticalOffset = 1;

void main(void) {

    mat3 TBN = mat3(normalize(IN.tangent), normalize(IN.binormal), normalize(IN.normal));

    vec3 normal = (texture(normalTex, IN.texCoord).rgb * 2.0) - 1.0;
    normal = normalize(TBN * normalize(normal));

    vec4 base = texture(baseTex, IN.texCoord);
    base = vec4(pow(base.rgb, vec3(2.2)), base.a);

	fragColour[0] = base;
    
    float distFromCentre = length(IN.texCoord - vec2(0.5,0.5));

    //Handy tool for calculating wave modifiers: https://www.desmos.com/calculator/w9jrdpvsmk
    float overlayStrength = amplitude * sin( (distFromCentre + offset) / frequency ) + verticalOffset; 

    vec4 circuit = texture(circuitTex, IN.texCoord);
    circuit = vec4(pow(circuit.rgb, vec3(2.2)), circuit.a);
    vec4 overlay = texture(overlayTex, IN.texCoord);
    overlay = vec4(pow(overlay.rgb, vec3(2.2)), overlay.a);


    fragColour[0] += circuit * overlay * overlayStrength;
    fragColour[1] = vec4(normal.xyz * 0.5 + 0.5, 1.0);
}

