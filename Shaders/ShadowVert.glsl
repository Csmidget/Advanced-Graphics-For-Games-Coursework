#version 330 core
uniform mat4 modelMatrix;

in vec3 position;

void main(void) {
    gl_Position = modelMatrix * vec4(position, 1.0);
}