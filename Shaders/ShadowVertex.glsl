#version 330 core
uniform mat4 modelMatrix;

in vec3 position;
in vec4 jointWeights;
in ivec4 jointIndices;

uniform mat4 joints[128];
uniform bool hasJoints;

void main(void) {

    vec4 localPos = vec4(position, 1.0f);
    vec4 skelPos = vec4(0,0,0,0);

    if (hasJoints) {
        for (int i = 0; i < 4; ++i) {
            int jointIndex = jointIndices[i];
            float jointWeight = jointWeights[i];

            skelPos += joints[jointIndex] * localPos * jointWeight;
        }
       gl_Position = modelMatrix * vec4(skelPos.xyz, 1.0);
    } 
    else {
        gl_Position = modelMatrix * vec4(position, 1.0);
    }

}