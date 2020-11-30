#version 330 core
uniform sampler2D diffuseTex;
uniform sampler2D diffuseLight;
uniform sampler2D transparentDiffuseTex;
uniform sampler2D specularLight;
uniform sampler2D skyboxTex;
uniform sampler2D neonGridTex;
uniform float ambient;
uniform float transparentAmbient;

in Vertex {
    vec2 texCoord;
} IN;

out vec4 fragColour;

void main(void) {
    vec4 diffuse = texture(diffuseTex, IN.texCoord);
    vec4 transparentDiffuse = texture(transparentDiffuseTex, IN.texCoord);
    vec4 light = texture(diffuseLight, IN.texCoord);
    vec3 specular = texture(specularLight,IN.texCoord).xyz;
    vec3 skyBox = texture(skyboxTex,IN.texCoord).xyz;
    vec3 neonGridTex = texture(neonGridTex, IN.texCoord).xyz;

    diffuse = (1 - transparentDiffuse.a) * diffuse;
    specular = (1 - transparentDiffuse.a) * specular;

    fragColour.xyz = diffuse.xyz * ambient;
    fragColour.xyz += diffuse.xyz * light.xyz;
    fragColour.xyz += specular;
    fragColour.xyz += transparentDiffuse.xyz * transparentAmbient;// * 0.1;
    fragColour.xyz += transparentDiffuse.xyz * light.xyz;
    fragColour.xyz += skyBox;
    fragColour.xyz += neonGridTex;
    fragColour.a = 1.0f;
}