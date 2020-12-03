#version 330 core

//Gaussian blur

uniform sampler2D sceneTex;

uniform int isVertical;

in Vertex {
    vec2 texCoord;
} IN;

out vec4 fragColor;

const float scaleFactors[7] = float[](0.006,0.061,0.242,0.383,0.242,0.061,0.006);

void main(void) {
    fragColor = vec4(0,0,0,1);
    vec2 delta = vec2(0,0);

    if (isVertical == 1) {
        delta = dFdy(IN.texCoord);
    } 
    else {
        delta = dFdx(IN.texCoord);
    }
    
    for(int i = 0; i < 7; i++) {
        vec2 offset = delta * (i - 3);
        vec3 tmp = texture(sceneTex, IN.texCoord.xy + offset).rgb;
        fragColor += vec4(tmp * scaleFactors[i],0.0);
    }
    fragColor.a = 1.0;
}