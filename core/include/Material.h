#pragma once

#include "../include/OpenGLRenderer.h"

struct Material {
	Material();
	void SetToProgram(GLuint program) const;

	glm::vec4		m_cAmbient;
	glm::vec4		m_cDiffuse;
	glm::vec4		m_cSpecular;
	glm::vec4		m_cEmissive;

	float			m_fSpecularPower;
};