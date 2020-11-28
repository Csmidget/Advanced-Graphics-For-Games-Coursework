#version 330 core

uniform sampler2D depthTex;
uniform sampler2D normalTex;
uniform samplerCube shadowMap;

uniform vec2 pixelSize;
uniform vec3 cameraPos;

uniform float lightRadius;
uniform vec3 lightPos;
uniform vec4 diffuseColour;
uniform mat4 inverseProjView;

out vec4 diffuseOutput;
out vec4 specularOutput;

void main(void) {
    vec2 texCoord = vec2(gl_FragCoord.xy * pixelSize);
    float depth = texture(depthTex, texCoord.xy).r;
    vec3 ndcPos = vec3(texCoord, depth) * 2.0 - 1.0;
    vec4 invClipPos = inverseProjView * vec4(ndcPos, 1.0);
    vec3 worldPos = invClipPos.xyz / invClipPos.w;

    vec3 worldPosToLightPos = worldPos - lightPos;
    float dist = length(worldPosToLightPos);
    float atten = 1.0 - clamp( dist / lightRadius, 0.0, 1.0);
    //float atten = 1.0 - clamp(1 / pow(lightRadius / dist, 2), 0.0, 1.0);

    if (atten == 0.0) {
        discard;
    }

    vec3 normal = normalize(texture(normalTex, texCoord.xy).xyz * 2.0 - 1.0);
    vec3 incident = normalize(lightPos - worldPos);
    vec3 viewDir = normalize(cameraPos - worldPos);
    vec3 halfDir = normalize(incident + viewDir);

    float lambert = clamp(dot(incident, normal),0.0,1.0);
    float rFactor = clamp(dot(halfDir, normal),0.0,1.0);
    float specFactor = rFactor;
    specFactor = pow(specFactor, 60.0);

    float shadowMapDepth = texture(shadowMap, worldPosToLightPos).r;
    shadowMapDepth *= lightRadius;

    float bias = 0.3;
    float shadow = dist - bias > shadowMapDepth ? 0.0 : 1.0;

    vec3 attenuated = diffuseColour.xyz * atten;

    diffuseOutput = vec4(attenuated * lambert * shadow, 1.0);
    specularOutput = vec4(attenuated * specFactor * 0.33 * shadow, 1.0);
}
