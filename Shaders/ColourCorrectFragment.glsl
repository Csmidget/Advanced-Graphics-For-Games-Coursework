#version 330 core
uniform sampler2D diffuseTex;

in Vertex {
    vec2 texCoord;
} IN;

out vec4 fragColour;

void main(void) {
    vec3 texRGB = texture(diffuseTex, IN.texCoord.xy).rgb;

    float luminanceIN = dot(texRGB, vec3(0.2126,0.7152,0.0722));
    float luminanceOUT = luminanceIN / (luminanceIN + 1);

    vec3 rgbOut = (luminanceOUT / luminanceIN) * texRGB;

    fragColour = vec4(rgbOut, 1.0f);
    fragColour.a = 1.0f;
}