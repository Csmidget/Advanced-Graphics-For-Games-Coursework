#version 330 core

uniform samplerCube cubeTex;
in Vertex {
    vec3 viewDir;
} IN;

out vec4 fragColour;

void main(void) {

    vec4 diffuse = texture(cubeTex, normalize(IN.viewDir));
    diffuse = vec4(pow(diffuse.rgb, vec3(2.2)), diffuse.a);

    fragColour = diffuse;
}