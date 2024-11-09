#pragma once

#include "../include/Node.h"

// forward declarations
class Geometry;
struct Material;

class GeometryNode : public Node
{
public:
	GeometryNode(
		const std::shared_ptr<Geometry>& geometry,
		const std::shared_ptr<Material>& material) :
		m_pGeometry(geometry),
		m_pMaterial(material)
	{
		m_arrTextures = { 0, 0, 0, 0 };
		m_arrTextureWrapModes = { GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE };
	}

	void Render(IRenderer& renderer, GLuint program) override;

	void SetGeometry(const std::shared_ptr<Geometry>& geometry) { m_pGeometry = geometry; }
	void SetMaterial(const std::shared_ptr<Material>& material) { m_pMaterial = material; }
	void SetTexture(GLuint slot, GLuint texture) { m_arrTextures[slot] = texture; }
	void SetTextures(const std::array<GLuint, 4>& textures) { m_arrTextures = textures; }
	void SetTextureWrapMode(GLuint slot, GLint mode) { m_arrTextureWrapModes[slot] = mode; }
	void SetTextureWrapModes(const std::array<GLint, 4>& modes) { m_arrTextureWrapModes = modes; }

protected:
	std::shared_ptr<Geometry>	m_pGeometry;
	std::shared_ptr<Material>	m_pMaterial;
	std::array<GLuint, 4>		m_arrTextures;
	std::array<GLint, 4>		m_arrTextureWrapModes;
};