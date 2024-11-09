#include "TheApp.h"

// init members
TheApp::TheApp() :
	m_uVertexShader(0),
	m_uFragmentShader(0),
	m_uProgram(0)
{
}

// called when graphics initialization is done
bool TheApp::OnCreate()
{
	auto renderer = GetOpenGLRenderer();

	// Generate shaders,program and textures
	m_uVertexShader = renderer->CreateVertexShaderFromFile("phongshader.vert");
	m_uFragmentShader = renderer->CreateFragmentShaderFromFile("phongshader.frag");
	m_uProgram = renderer->CreateProgram(m_uVertexShader, m_uFragmentShader);
	m_uTexture = renderer->CreateTexture("earth.jpg");
	if (!m_uVertexShader || !m_uFragmentShader || !m_uProgram || !m_uTexture) {
		return false;
	}

	// set view and projection matrices
	renderer->SetViewMatrix(glm::lookAt(glm::vec3(0.0f, 0.0f, 20.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)));
	renderer->SetProjectionMatrix(glm::perspective(glm::radians(45.0f), GetAspect(), 1.0f, 500.0f));

	// Generate geometry
	const float radius = 1.0f;
	m_pSphere = std::make_shared<Geometry>();
	m_pSphere->GenSphere(glm::vec3(radius));

	// init material
	m_pMaterial = std::make_shared<Material>();
	m_pMaterial->m_cAmbient = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	m_pMaterial->m_cDiffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	m_pMaterial->m_cSpecular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	//m_pMaterial->m_cEmissive = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

	// create scene root
	m_pSceneRoot = std::make_unique<Node>();

	// generate random spheres and add them to scene
	for (size_t i = 0; i < 20; ++i)
	{
		auto node = std::make_shared<GeometryNode>(m_pSphere, m_pMaterial);
		node->SetTexture(0, m_uTexture);

		node->SetPos(glm::vec3(
			glm::linearRand(-5.0f, 5.0f),
			glm::linearRand(-5.0f, 5.0f),
			glm::linearRand(-5.0f, 5.0f)));

		if (glm::linearRand(0, 5) == 0) {
			for (size_t j = 0; j < 5; ++j) {
				auto node2 = std::make_shared<GeometryNode>(m_pSphere, m_pMaterial);
				node2->SetTexture(0, m_uTexture);
				node2->SetPos({ 0.0f, radius * (j + 1) * 2.0f, 0.0f });
				SetRandomRotation(*node2);
				node->AddNode(node2);
			}
			SetRandomRotation(*node);
		}

		/*
		node->SetVelocity({ 0.0f, 10.0f, 0.0f });
		node->SetAcceleration({ 0.0f, -9.81, 0.0f });
		*/

		m_pSceneRoot->AddNode(node);
	}

	return true;
}

// called when app is about to close
void TheApp::OnDestroy()
{
	// clear all resources
	m_pSceneRoot = nullptr;
	m_pSphere = nullptr;

	glDeleteTextures(1, &m_uTexture);
	glDeleteProgram(m_uProgram);
	glDeleteShader(m_uVertexShader);
	glDeleteShader(m_uFragmentShader);
}

void TheApp::OnUpdate(float frametime)
{
	// update scene and all its children
	if (m_pSceneRoot) {
		m_pSceneRoot->Update(frametime);

		/*
		if (IsKeyDown(VK_LEFT)) {
			m_pSceneRoot->SetVelocity({ -5.0f, 0.0f, 0.0f });
		}
		else if (IsKeyDown(VK_RIGHT)) {
			m_pSceneRoot->SetVelocity({ 5.0f, 0.0f, 0.0f });
		}
		else {
			m_pSceneRoot->SetVelocity({ 0.0f, 0.0f, 0.0f });
		}

		auto& nodes = m_pSceneRoot->GetNodes();
		for (auto& node : nodes) {
			auto pos = node->GetPos();
			if (pos.y < -4.0f) {
				pos.y = -4.0f;
				node->Velocity() *= 5.0f;
				node->SetPos(pos);

				if (glm::length(node->Velocity()) < 0.5f) {
					node->SetRotationSpeed(0.0f);
				}
				else {
					SetRandomRotation(*node);
				}
			}
		}
		*/
	}
}

void TheApp::OnDraw(IRenderer& renderer)
{
	// clear depth and stencil buffers
	renderer.Clear(0.2f, 0.2f, 0.2f, 1.0f);

	// set program
	glUseProgram(m_uProgram);

	// set camera position
	OpenGLRenderer::SetUniformVec3(m_uProgram, "cameraPosition", -renderer.GetViewMatrix()[3]);

	// set light direction
	const glm::vec3 lightDirection(glm::normalize(glm::vec3(1.0f, 0.1f, -1.0f)));
	OpenGLRenderer::SetUniformVec3(m_uProgram, "lightDirection", lightDirection);

	// render scene and all its children
	if (m_pSceneRoot) {
		m_pSceneRoot->Render(renderer, m_uProgram);
	}
}

// called when screen size is changed
void TheApp::OnScreenChanged(int32_t widthPixels, int32_t heightPixels)
{
	// reset projection matrix on screen size change
	GetRenderer()->SetProjectionMatrix(glm::perspective(glm::radians(45.0f), GetAspect(), 1.0f, 500.0f));
}

// handle key down events
bool TheApp::OnKeyDown(int32_t keyCode)
{
	if (keyCode == VK_ESCAPE) {
		Close();
		return true;
	}

	return false;
}

// apply random rotation to selected node
void TheApp::SetRandomRotation(Node& node) const
{
	const glm::vec3 axis(glm::normalize(glm::vec3(
		glm::linearRand(-1.0f, 1.0f),
		glm::linearRand(-1.0f, 1.0f),
		glm::linearRand(-1.0f, 1.0f)
	)));
	node.SetRotationAxis(axis);
	node.SetRotationAngle(glm::linearRand(0.0f, glm::two_pi<float>()));
	node.SetRotationSpeed(glm::linearRand(-5.0f, 5.0f));
}