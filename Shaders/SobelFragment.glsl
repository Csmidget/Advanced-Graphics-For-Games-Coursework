#version 330 core

uniform sampler2D sceneTex;

in Vertex {
    vec2 texCoord;
} IN;

out vec4 fragColor;

const float gx[9] = float[](1,0,-1,2,0,-2,1,0,-1);
const float gy[9] = float[](-1,-2,-1,0,0,0,1,2,1);

void main(void) {

    vec2 delta = dFdy(IN.texCoord) + dFdx(IN.texCoord);
    vec4 sobelX = vec4(0,0,0,0);
    vec4 sobelY = vec4(0,0,0,0);
    
    for(int x = 0; x < 3; x++) {
        for(int y = 0; y < 3; y++) {
            vec2 offset = vec2(delta.x * (x-1), delta.y * (y-1));
            vec4 texCol = texture(sceneTex, IN.texCoord.xy + offset);
            sobelX += gx[y*3 + x] * texCol;
            sobelY += gy[y*3 + x] * texCol;
        }
    }

    fragColor = sqrt(sobelX*sobelX + sobelY * sobelY);
}