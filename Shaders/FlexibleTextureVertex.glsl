#version 420 core

uniform mat4 modelMatrix;
uniform mat4 textureMatrix;

uniform mat4 joints[128];
uniform bool hasJoints;

layout(std140, binding = 1) uniform ProjView {
	mat4 projMatrix;
	mat4 viewMatrix;
} PV;

in vec3 position;
in vec2 texCoord;
in vec4 jointWeights;
in ivec4 jointIndices;

out Vertex {
	vec2 texCoord;
} OUT;

void main(void) {
	mat4 mvp = PV.projMatrix * PV.viewMatrix * modelMatrix;

    vec4 localPos = vec4(position, 1.0f);
    vec4 skelPos = vec4(0,0,0,0);

    if (hasJoints) {
        for (int i = 0; i < 4; ++i) {
            int jointIndex = jointIndices[i];
            float jointWeight = jointWeights[i];

            skelPos += joints[jointIndex] * localPos * jointWeight;
        }
       gl_Position =  mvp * vec4(skelPos.xyz, 1.0);
    } else {
        gl_Position =  mvp * localPos;
    }

	OUT.texCoord = (textureMatrix * vec4(texCoord, 0.0, 1.0)).xy;
}