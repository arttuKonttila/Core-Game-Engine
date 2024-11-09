#include "../include/GeometryNode.h"
#include "../include/Geometry.h"
#include "../include/Material.h"

void GeometryNode::Render(IRenderer& renderer, GLuint program)
{
	if (m_pGeometry) {
		m_pGeometry->SetAttribs(program);

		// set world matrix
		const glm::mat4 worldMatrix(GetWorldMatrix());
		OpenGLRenderer::SetUniformMatrix4(program, "modelMatrix", worldMatrix);

		// set model view projection matrix
		const glm::mat4 mvp(renderer.GetProjectionMatrix() * renderer.GetViewMatrix() * worldMatrix);
		OpenGLRenderer::SetUniformMatrix4(program, "modelViewProjectionMatrix", mvp);

		// set material
		if (m_pMaterial) {
			m_pMaterial->SetToProgram(program);
		}

		// set textures from texture array
		char name[16] = "texture01";
		for (uint32_t slot = 0; slot < m_arrTextures.size(); ++slot) {
			auto texture = m_arrTextures[slot];
			if (texture) {
				renderer.SetTexture(program, texture, slot, name);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_arrTextureWrapModes[slot]);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_arrTextureWrapModes[slot]);
			}

			// increment texture name
			++name[8];
		}

		// draw geometry
		m_pGeometry->Draw(renderer);
	}

	// render children
	Node::Render(renderer, program);
}