#version 430 core
uniform sampler2D diffuseTex;
uniform float saturationPoint;

in Vertex {
    vec2 texCoord;
} IN;

out vec4 fragColour;

void main(void) {
    vec3 texRGB = texture(diffuseTex, IN.texCoord.xy).rgb;

   // int mipMapLevel = textureQueryLevels(diffuseTex);
   // vec3 avgLuminance = textureLod(diffuseTex, vec2(0,0), mipMapLevel).rgb;
   // vec3 rgbOut = vec3(1.0) - exp(-texRGB * saturationPoint);// texRGB / (texRGB + vec3(1.0));  // (texRGB * (1 + texRGB / pow(saturationPoint,2)) )/ (texRGB + 1);

    float luminanceIN = dot(texRGB, vec3(0.2126,0.7152,0.0722));
    float luminanceOUT = (luminanceIN * (1 + luminanceIN / pow(saturationPoint,2)) )/ (luminanceIN + 1);

    vec3 rgbOut = (luminanceOUT / luminanceIN) * texRGB;

   // rgbOut = pow(rgbOut, vec3(1 / 2.2));

    fragColour = vec4(rgbOut, 1.0f);
    fragColour.a = 1.0f;
}