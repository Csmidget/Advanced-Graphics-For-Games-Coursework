#version 330 core

uniform sampler2D diffuseTex1;
uniform sampler2D normalTex1;
uniform sampler2D diffuseTex2;
uniform sampler2D normalTex2;
uniform sampler2D assignMap;
uniform vec3      dimensions;

in Vertex {
    vec4 colour;
    vec2 texCoord;
    vec3 normal;
    vec3 tangent;
    vec3 binormal;
    vec3 localPos;
    vec3 worldPos;
} IN;

out vec4 fragColour[2];

void main(void) {
    mat3 TBN = mat3(normalize(IN.tangent), normalize(IN.binormal), normalize(IN.normal));

    vec2 assignPos = vec2(IN.localPos.x / dimensions.x,IN.localPos.z / dimensions.z);

    vec4 diffuse1 = texture(diffuseTex1, IN.texCoord);
    diffuse1 = vec4(pow(diffuse1.rgb, vec3(2.2)), diffuse1.a);

    vec4 diffuse2 = texture(diffuseTex2, IN.texCoord);
    diffuse2 = vec4(pow(diffuse2.rgb, vec3(2.2)), diffuse2.a);

    vec3 normal1 = (texture(normalTex1, IN.texCoord).rgb * 2.0) - 1.0;
    vec3 normal2 = (texture(normalTex2, IN.texCoord).rgb * 2.0) - 1.0;
    vec3 assign = texture(assignMap, assignPos).rgb;

    vec3 normal = (normal1 * assign.r) + (normal2 * assign.g);
    normal = normalize(TBN * normalize(normal));

    vec4 diffuse = (diffuse1 * assign.r) + (diffuse2 * assign.g);

    fragColour[0] = diffuse;
    fragColour[1] = vec4(normal.xyz * 0.5 + 0.5, 1.0);
}