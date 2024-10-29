#pragma once

#include <vector>
#include "../include/OpenGLRenderer.h"

class Geometry {
public:
	struct VERTEX {
		VERTEX() :
			x(0.0f), y(0.0f), z(0.0f),
			nx(0.0f), ny(0.0f), nz(0.0f),
			tu(0.0f), tv(0.0f)

		{}
		VERTEX(float _x, float _y, float _z, float _nx, float _ny, float _nz, float _tu, float _tv) :
			x(_x), y(_y), z(_z),
			nx(_nx), ny(_ny), nz(_nz),
			tu(_tu), tv(_tv)
		{}
		VERTEX(const glm::vec3& pos, const glm::vec3& normal, float _tu, float _tv) :
			x(pos.x), y(pos.y), z(pos.z),
			nx(normal.x), ny(normal.y), nz(normal.z),
			tu(_tu), tv(_tv)
		{}

		static constexpr int32_t GetStride() { return 32; }

		float x, y, z;
		float nx, ny, nz;
		float tu, tv;
	};

	Geometry();
	~Geometry();

	void Clear();

	void GenSphere(const glm::vec3& radius,
		const glm::vec3& offset = glm::vec3(0.0f),
		uint32_t rings = 24,
		uint32_t segments = 24
	);

	void SetAttribs(GLuint program);
	void DisableAttribs(GLuint program);

	void Draw(IRenderer& renderer);

private:
	std::vector<VERTEX>			m_arrVertices;
	GLenum						m_eDrawMode;
};