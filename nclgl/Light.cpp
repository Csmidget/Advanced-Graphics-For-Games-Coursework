#include "Light.h"
#include "Shader.h"
#include "OGLRenderer.h"


GLuint Light::GenerateShadowMapTexture() {
	
	//If we already have a texture bound don't make a new one...
	if (shadowMap != 0)
		return shadowMap;

	glGenTextures(1, &shadowMap);

	glBindTexture(GL_TEXTURE_CUBE_MAP, shadowMap);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	int shadowRes = isStatic ? STATIC_SHADOW_RESOLUTION : DYNAMIC_SHADOW_RESOLUTION;

	for (unsigned int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
			shadowRes, shadowRes, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);
	
	return shadowMap;
}

void PointLight::SetShaderLightData(const Shader* shader) {
	glUniform3fv(glGetUniformLocation(shader->GetProgram(), "lightPos"), 1, (float*)&GetPosition());

	glUniform4fv(glGetUniformLocation(shader->GetProgram(), "diffuseColour"), 1, (float*)&GetDiffuseColour());
	glUniform4fv(glGetUniformLocation(shader->GetProgram(), "specularColour"), 1, (float*)&GetSpecularColour());
	glUniform1f(glGetUniformLocation(shader->GetProgram(), "lightFarPlane"), radius);
	glUniform1f(glGetUniformLocation(shader->GetProgram(), "lightRadius"), radius);
}

void SpotLight::SetShaderLightData(const Shader* shader) {

	Matrix4 rotMat = Matrix4::Rotation(rotation.z, Vector3(0, 0, 1)) *
					 Matrix4::Rotation(rotation.y, Vector3(0, 1, 0)) *
					 Matrix4::Rotation(rotation.x, Vector3(1, 0, 0)) *
					 Matrix4::Translation({0,-1,0});

		Vector3 dir = rotMat * Vector3(0, 0, 0);
	//dir.Normalise();
	glUniform3fv(glGetUniformLocation(shader->GetProgram(), "lightDir"), 1, (float*)&dir);
	glUniform3fv(glGetUniformLocation(shader->GetProgram(), "lightPos"), 1, (float*)&GetPosition());

	glUniform4fv(glGetUniformLocation(shader->GetProgram(), "diffuseColour"), 1, (float*)&GetDiffuseColour());
	glUniform4fv(glGetUniformLocation(shader->GetProgram(), "specularColour"), 1, (float*)&GetSpecularColour());

	glUniform1f(glGetUniformLocation(shader->GetProgram(), "lightConeAngle"), DegToRad(coneAngle));
	glUniform1f(glGetUniformLocation(shader->GetProgram(), "lightRadius"), radius);
	glUniform1f(glGetUniformLocation(shader->GetProgram(), "lightFarPlane"), radius);

}

void DirectionLight::SetShaderLightData(const Shader* shader) {

	glUniform3fv(glGetUniformLocation(shader->GetProgram(), "lightDir"), 1, (float*)&direction);

	glUniform4fv(glGetUniformLocation(shader->GetProgram(), "diffuseColour"), 1, (float*)&GetDiffuseColour());
	glUniform4fv(glGetUniformLocation(shader->GetProgram(), "specularColour"), 1, (float*)&GetSpecularColour());
}