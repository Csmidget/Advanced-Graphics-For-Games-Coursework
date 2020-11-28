#include "Light.h"
#include "Shader.h"


void PointLight::SetShaderLightData(const Shader* shader) {
	glUniform3fv(glGetUniformLocation(shader->GetProgram(), "lightPos"), 1, (float*)&GetPosition());

	glUniform4fv(glGetUniformLocation(shader->GetProgram(), "diffuseColour"), 1, (float*)&GetDiffuseColour());
	glUniform4fv(glGetUniformLocation(shader->GetProgram(), "specularColour"), 1, (float*)&GetSpecularColour());

	glUniform1f(glGetUniformLocation(shader->GetProgram(), "lightRadius"), radius);
}

void SpotLight::SetShaderLightData(const Shader* shader) {

	Matrix4 rotMat = Matrix4::Rotation(rotation.x, Vector3(1, 0, 0)) *
		Matrix4::Rotation(rotation.y, Vector3(0, 1, 0)) *
		Matrix4::Rotation(rotation.z, Vector3(0, 0, 1));

	Vector3 dir = rotMat * direction;
	glUniform3fv(glGetUniformLocation(shader->GetProgram(), "lightDir"), 1, (float*)&dir);
	glUniform3fv(glGetUniformLocation(shader->GetProgram(), "lightPos"), 1, (float*)&GetPosition());

	glUniform4fv(glGetUniformLocation(shader->GetProgram(), "diffuseColour"), 1, (float*)&GetDiffuseColour());
	glUniform4fv(glGetUniformLocation(shader->GetProgram(), "specularColour"), 1, (float*)&GetSpecularColour());
	glUniform1f(glGetUniformLocation(shader->GetProgram(), "lightConeAngle"), DegToRad(coneAngle));
	glUniform1f(glGetUniformLocation(shader->GetProgram(), "lightRadius"), radius);
}

void DirectionLight::SetShaderLightData(const Shader* shader) {

	glUniform3fv(glGetUniformLocation(shader->GetProgram(), "lightDir"), 1, (float*)&direction);

	glUniform4fv(glGetUniformLocation(shader->GetProgram(), "diffuseColour"), 1, (float*)&GetDiffuseColour());
	glUniform4fv(glGetUniformLocation(shader->GetProgram(), "specularColour"), 1, (float*)&GetSpecularColour());
}