#version 330 core

uniform sampler2D diffuseTex;
uniform vec3 cameraPos;
uniform vec4[10] lightDiffuseColours;
uniform vec4[10] lightSpecularColours;
uniform vec3[10] lightPositions;
uniform float[10] lightRadiuses;

in Vertex {
    vec3 colour;
    vec2 texCoord;
    vec3 normal;
    vec3 worldPos;
} IN;

out vec4 fragColour;

void main(void) {

    vec3 viewDir = normalize(cameraPos - IN.worldPos);
    vec4 diffuse = texture(diffuseTex, IN.texCoord);

    for(int i = 0; i < 10; i++) {
        vec3 incident = normalize(lightPositions[i] - IN.worldPos);
        vec3 halfDir = normalize(incident + viewDir);

        float lambert = max(dot(incident, IN.normal), 0.0f);
        float distance = length(lightPositions[i] - IN.worldPos);

        float attenuation = clamp(1 / pow(distance, 2), 0.0, 1.0); //1.0 - clamp(distance / lightRadius, 0.0, 1.0);
        float specFactor = clamp(dot(halfDir,IN.normal),0.0,1.0);
        specFactor = pow(specFactor, 60.0);

        vec3 surface = (diffuse.rgb * lightDiffuseColours[i].rgb);
        fragColour.rgb += surface * lambert * attenuation * lightDiffuseColours[i].w; 
        fragColour.rgb += (lightSpecularColours[i].rgb * specFactor * lightSpecularColours[i].w) * attenuation * 0.33;
        fragColour.rgb += surface * 0.01f;
    }

    fragColour.a = diffuse.a;
}