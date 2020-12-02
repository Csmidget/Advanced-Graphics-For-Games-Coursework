#version 330 core

uniform sampler2D diffuseTex;

in Vertex {
    vec4 colour;
    vec2 texCoord;
    vec3 normal;
} IN;

out vec4 fragColour[2];

void main(void) {
    
    vec4 diffuse = texture(diffuseTex, IN.texCoord);
    diffuse = vec4(pow(diffuse.rgb, vec3(2.2)), diffuse.a);
    fragColour[0] = diffuse;
    fragColour[1] = vec4(IN.normal.xyz * 0.5 + 0.5, 1.0);
}