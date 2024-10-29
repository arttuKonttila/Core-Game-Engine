#include "../include/Geometry.h"

Geometry::Geometry() :
	m_eDrawMode(GL_TRIANGLES)
{
}

Geometry::~Geometry()
{
	Clear();
}

void Geometry::Clear()
{
	m_arrVertices.clear();
}

void Geometry::GenSphere(const glm::vec3& radius, const glm::vec3& offset, uint32_t rings, uint32_t segments)
{
	Clear();
	m_eDrawMode = GL_TRIANGLE_STRIP;

	const float deltaRingAngle = (glm::pi<float>() / rings);
	const float deltaSegAngle = (glm::two_pi<float>() / segments);

	for (uint32_t ring = 0; ring < rings; ++ring) {
		const float r0 = sinf((ring + 0) * deltaRingAngle);
		const float r1 = sinf((ring + 1) * deltaRingAngle);
		const float y0 = cosf((ring + 0) * deltaRingAngle);
		const float y1 = cosf((ring + 1) * deltaRingAngle);

		for (uint32_t seg = 0; seg < (segments + 1); ++seg) {
			const float angle = seg * deltaSegAngle;
			const float x0 = r0 * sinf(angle);
			const float z0 = r0 * cosf(angle);
			const float x1 = r1 * sinf(angle);
			const float z1 = r1 * cosf(angle);

			m_arrVertices.push_back(VERTEX(
				radius.x * x1 + offset.x,
				radius.y * y1 + offset.y,
				radius.z * z1 + offset.z,
				x1,
				y1,
				z1,
				((float)seg) / segments,
				(ring + 1) / (float)rings
			));

			m_arrVertices.push_back(VERTEX(
				radius.x * x0 + offset.x,
				radius.y * y0 + offset.y,
				radius.z * z0 + offset.z,
				x0,
				y0,
				z0,
				((float)seg) / segments,
				(ring / (float)rings)
			));
		}
	}
}

void Geometry::SetAttribs(GLuint program)
{
	const GLint position = glGetAttribLocation(program, "position");
	const GLint normal = glGetAttribLocation(program, "normal");
	const GLint uv = glGetAttribLocation(program, "uv");

	const float* vertexData = (const float*)m_arrVertices.data();

	glEnableVertexAttribArray(position);
	glEnableVertexAttribArray(uv);
	glEnableVertexAttribArray(normal);
	glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, VERTEX::GetStride(), vertexData);
	glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, VERTEX::GetStride(), vertexData + 3);
	glVertexAttribPointer(uv, 2, GL_FLOAT, GL_FALSE, VERTEX::GetStride(), vertexData + 6);
}

void Geometry::DisableAttribs(GLuint program)
{
	const GLint position = glGetAttribLocation(program, "position");
	const GLint normal = glGetAttribLocation(program, "normal");
	const GLint uv = glGetAttribLocation(program, "uv");
	glDisableVertexAttribArray(position);
	glDisableVertexAttribArray(normal);
	glDisableVertexAttribArray(uv);
}

void Geometry::Draw(IRenderer& renderer)
{
	glDrawArrays(m_eDrawMode, 0, (GLsizei)m_arrVertices.size());
}