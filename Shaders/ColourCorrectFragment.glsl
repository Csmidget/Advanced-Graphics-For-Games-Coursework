#version 430 core
uniform sampler2D diffuseTex;
uniform float saturationPoint;

in Vertex {
    vec2 texCoord;
} IN;

out vec4 fragColour;

void main(void) {
    vec3 texRGB = texture(diffuseTex, IN.texCoord.xy).rgb;

    float luminanceIN = dot(texRGB, vec3(0.2126,0.7152,0.0722));
    float luminanceOUT = (luminanceIN * (1 + luminanceIN / pow(saturationPoint,2)) )/ (luminanceIN + 1);

    vec3 rgbOut = (luminanceOUT / luminanceIN) * texRGB;

    rgbOut = pow(rgbOut, vec3(1 / 2.2));

    fragColour = vec4(rgbOut, 1.0f);
    fragColour.a = 1.0f;
}