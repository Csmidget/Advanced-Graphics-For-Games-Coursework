#version 330 core
uniform sampler2D diffuseTex;
uniform sampler2D diffuseLight;
uniform sampler2D transparentDiffuseTex;
uniform sampler2D specularLight;
uniform sampler2D skyboxTex;

in Vertex {
    vec2 texCoord;
} IN;

out vec4 fragColour;

void main(void) {
    vec3 diffuse = texture(diffuseTex, IN.texCoord).xyz;
    vec4 transparentDiffuse = texture(transparentDiffuseTex, IN.texCoord);
    vec3 light = texture(diffuseLight, IN.texCoord).xyz;
    vec3 specular = texture(specularLight,IN.texCoord).xyz;
    vec3 skyBox = texture(skyboxTex,IN.texCoord).xyz;

    diffuse = (1 - transparentDiffuse.a) * diffuse;
    specular = (1 - transparentDiffuse.a) * specular;

    fragColour.xyz = diffuse * 0.1;
    fragColour.xyz += diffuse * light;
    fragColour.xyz += specular;
    fragColour.xyz += transparentDiffuse.xyz;// * 0.1;
   // fragColour.xyz += transparentDiffuse.xyz * light;
    fragColour.xyz += skyBox;
    fragColour.a = 1.0;
}