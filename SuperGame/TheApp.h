#pragma once
#include "../core/include/IApplication.h"
#include "../core/include/GeometryNode.h"
#include "../core/include/Geometry.h"
#include "../core/include/Material.h"

class TheApp :
	public IApplication
{
public:
	TheApp();

	bool OnCreate() override;
	void OnDestroy() override;
	void OnUpdate(float frametime) override;
	void OnDraw(IRenderer& renderer) override;
protected:
	void OnScreenChanged(int32_t widthPixels, int32_t heightPixels) override;
	bool OnKeyDown(int32_t keyCode) override;
private:
	OpenGLRenderer* GetOpenGLRenderer() { return static_cast<OpenGLRenderer*>(GetRenderer()); }
	void SetRandomRotation(Node& node) const;

	GLuint							m_uVertexShader;
	GLuint							m_uFragmentShader;
	GLuint							m_uProgram;

	GLuint							m_uTexture;

	std::shared_ptr<Geometry>		m_pSphere;
	std::shared_ptr<Material>		m_pMaterial;

	std::unique_ptr<Node>			m_pSceneRoot;
};
