#version 330 core

uniform vec3 lightPos;
uniform float lightFarPlane;

in vec4 fragPos;

void main(void) {
    float lightDistance = length(fragPos.xyz - lightPos);

    lightDistance = lightDistance / lightFarPlane;

    gl_FragDepth = lightDistance;
}