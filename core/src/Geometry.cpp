/**
 * ============================================================================
 *  Name        : Geometry.cpp
 *  Part of     : Simple OpenGL graphics engine framework
 *  Description : 3d geometry generator and storage
 *  Version     : 1.00
 *	Author      : Jani Immonen, <realdashdev@gmail.com>
 * ============================================================================
**/

#include "../include/Geometry.h"

//#define TINYOBJLOADER_IMPLEMENTATION
//#define TINYOBJLOADER_USE_MAPBOX_EARCUT
//#include "tiny_obj_loader.h"

Geometry::Geometry() :
	m_IndexBuffer(0),
	m_uIndexCount(0),
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
	if (m_IndexBuffer)
	{
		glDeleteBuffers(1, &m_IndexBuffer);
		m_IndexBuffer = 0;
	}
	m_uIndexCount = 0;
}

void Geometry::GenSphere(const glm::vec3& radius, const glm::vec3& offset, uint32_t rings, uint32_t segments)
{
	Clear();
	m_eDrawMode = GL_TRIANGLE_STRIP;

	const float deltaRingAngle = (glm::pi<float>() / rings);
	const float deltaSegAngle = (glm::two_pi<float>() / segments);

	for (uint32_t ring = 0; ring < rings; ++ring)
	{
		const float r0 = sinf((ring + 0) * deltaRingAngle);
		const float r1 = sinf((ring + 1) * deltaRingAngle);
		const float y0 = cosf((ring + 0) * deltaRingAngle);
		const float y1 = cosf((ring + 1) * deltaRingAngle);

		for (uint32_t seg = 0; seg < (segments + 1); ++seg)
		{
			const float angle = seg * deltaSegAngle;
			const float x0 = r0 * sinf(angle);
			const float z0 = r0 * cosf(angle);
			const float x1 = r1 * sinf(angle);
			const float z1 = r1 * cosf(angle);

			m_arrVertices.emplace_back(VERTEX(
				radius.x * x1 + offset.x, radius.y * y1 + offset.y, radius.z * z1 + offset.z,
				x1, y1, z1,
				((float)seg) / segments,
				(ring + 1) / (float)rings));

			m_arrVertices.emplace_back(VERTEX(
				radius.x * x0 + offset.x, radius.y * y0 + offset.y, radius.z * z0 + offset.z,
				x0, y0, z0,
				((float)seg) / segments,
				(ring / (float)rings)));
		}
	}
}

void Geometry::GenCube(const glm::vec3& size, const glm::vec3& offset)
{
	Clear();
	m_eDrawMode = GL_TRIANGLES;

	const glm::vec3 nor1(1.0f, 0.0f, 0.0f);
	const glm::vec3 nor2(0.0f, 1.0f, 0.0f);
	const glm::vec3 nor3(0.0f, 0.0f, 1.0f);

	// cube size
	const float fW = size.x * 0.5f;
	const float fH = size.y * 0.5f;
	const float fD = size.z * 0.5f;

	// cube corners
	glm::vec3 p[8];
	p[0] = glm::vec3(-fW, fH, fD);
	p[1] = glm::vec3(fW, fH, fD);
	p[2] = glm::vec3(fW, -fH, fD);
	p[3] = glm::vec3(-fW, -fH, fD);
	p[4] = glm::vec3(-fW, -fH, -fD);
	p[5] = glm::vec3(fW, -fH, -fD);
	p[6] = glm::vec3(fW, fH, -fD);
	p[7] = glm::vec3(-fW, fH, -fD);

	// apply offset
	for (int32_t i = 0; i < 8; ++i)
	{
		p[i] += offset;
	}

	// front face
	m_arrVertices.emplace_back(VERTEX(p[0], nor3, 0.0f, 0.0f));
	m_arrVertices.emplace_back(VERTEX(p[1], nor3, 1.0f, 0.0f));
	m_arrVertices.emplace_back(VERTEX(p[2], nor3, 1.0f, 1.0f));
	m_arrVertices.emplace_back(VERTEX(p[3], nor3, 0.0f, 1.0f));
	// right face
	m_arrVertices.emplace_back(VERTEX(p[1], nor1, 0.0f, 0.0f));
	m_arrVertices.emplace_back(VERTEX(p[6], nor1, 1.0f, 0.0f));
	m_arrVertices.emplace_back(VERTEX(p[5], nor1, 1.0f, 1.0f));
	m_arrVertices.emplace_back(VERTEX(p[2], nor1, 0.0f, 1.0f));
	// back face
	m_arrVertices.emplace_back(VERTEX(p[6], -nor3, 0.0f, 0.0f));
	m_arrVertices.emplace_back(VERTEX(p[7], -nor3, 1.0f, 0.0f));
	m_arrVertices.emplace_back(VERTEX(p[4], -nor3, 1.0f, 1.0f));
	m_arrVertices.emplace_back(VERTEX(p[5], -nor3, 0.0f, 1.0f));
	// left face
	m_arrVertices.emplace_back(VERTEX(p[7], -nor1, 0.0f, 0.0f));
	m_arrVertices.emplace_back(VERTEX(p[0], -nor1, 1.0f, 0.0f));
	m_arrVertices.emplace_back(VERTEX(p[3], -nor1, 1.0f, 1.0f));
	m_arrVertices.emplace_back(VERTEX(p[4], -nor1, 0.0f, 1.0f));
	// top face
	m_arrVertices.emplace_back(VERTEX(p[7], nor2, 0.0f, 0.0f));
	m_arrVertices.emplace_back(VERTEX(p[6], nor2, 1.0f, 0.0f));
	m_arrVertices.emplace_back(VERTEX(p[1], nor2, 1.0f, 1.0f));
	m_arrVertices.emplace_back(VERTEX(p[0], nor2, 0.0f, 1.0f));
	// bottom face
	m_arrVertices.emplace_back(VERTEX(p[3], -nor2, 0.0f, 0.0f));
	m_arrVertices.emplace_back(VERTEX(p[2], -nor2, 1.0f, 0.0f));
	m_arrVertices.emplace_back(VERTEX(p[5], -nor2, 1.0f, 1.0f));
	m_arrVertices.emplace_back(VERTEX(p[4], -nor2, 0.0f, 1.0f));

	// create indices to cube object
	uint32_t indices[36];

	for (int32_t i = 0, j = 0; i < 21; i += 4, j += 6)
	{
		indices[j] = (i + 0);
		indices[j + 1] = (i + 1);
		indices[j + 2] = (i + 2);
		indices[j + 3] = (i + 0);
		indices[j + 4] = (i + 2);
		indices[j + 5] = (i + 3);
	}

	glGenBuffers(1, &m_IndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * 4, indices, GL_STATIC_DRAW);
	m_uIndexCount = 36;
}

void Geometry::GenQuad(const glm::vec2& size, const glm::vec3& offset)
{
	Clear();
	m_eDrawMode = GL_TRIANGLES;

	const float w = size.x * 0.5f;
	const float h = size.y * 0.5f;

	// quad normal
	const glm::vec3 normal(0.0f, 0.0f, 1.0f);

	// quad corners
	glm::vec3 p[4] = {
		glm::vec3(w, -h,  0.0f),
		glm::vec3(-w, -h,  0.0f),
		glm::vec3(-w,  h,  0.0f),
		glm::vec3(w,  h,  0.0f),
	};

	// apply offset
	for (size_t i = 0; i < 4; ++i)
	{
		p[i] += offset;
	}

	m_arrVertices.emplace_back(VERTEX(p[0], normal, 1.0f, 1.0f));
	m_arrVertices.emplace_back(VERTEX(p[1], normal, 0.0f, 1.0f));
	m_arrVertices.emplace_back(VERTEX(p[2], normal, 0.0f, 0.0f));

	m_arrVertices.emplace_back(VERTEX(p[2], normal, 0.0f, 0.0f));
	m_arrVertices.emplace_back(VERTEX(p[3], normal, 1.0f, 0.0f));
	m_arrVertices.emplace_back(VERTEX(p[0], normal, 1.0f, 1.0f));
}

void Geometry::GenTorus(uint32_t segments, float radius, float fatness)
{
	Clear();
	m_eDrawMode = GL_TRIANGLES;

	const size_t vertexCount = segments * segments;
	m_uIndexCount = (segments - 1) * (segments - 1) * 6;
	m_arrVertices.resize(vertexCount);

	size_t index;

	// build the vertices
	for (size_t i = 0; i < segments; ++i)
	{
		float yPhase = (float)i / (float)(segments - 1);
		yPhase *= glm::two_pi<float>();

		const float nr = cosf(yPhase);
		const float ny = sinf(yPhase);
		const float r = radius + nr * fatness;
		const float y = ny * fatness;

		for (size_t j = 0; j < segments; ++j)
		{
			index = i * segments + j;

			float xPhase = (float)j / (float)(segments - 1);
			xPhase *= glm::two_pi<float>();

			const float cost = cosf(xPhase);
			const float sint = sinf(xPhase);

			m_arrVertices[index].x = cost * r;
			m_arrVertices[index].z = sint * r;
			m_arrVertices[index].y = y;
			m_arrVertices[index].nx = cost * nr;
			m_arrVertices[index].ny = ny;
			m_arrVertices[index].nz = sint * nr;

			glm::vec3& normal = (glm::vec3&)(m_arrVertices[index].nx);
			normal = glm::normalize(normal);

			m_arrVertices[index].tu = m_arrVertices[index].nx;
			m_arrVertices[index].tv = m_arrVertices[index].nz;
		}
	}

	std::vector<uint32_t> indices;
	indices.resize(m_uIndexCount);

	index = 0;
	for (size_t i = 0; i < segments - 1; i++)
	{
		for (size_t j = 0; j < segments - 1; j++)
		{
			// set of faces
			indices[index++] = (uint32_t)(i * segments + j + 1);
			indices[index++] = (uint32_t)((i + 1) * segments + j + 1);
			indices[index++] = (uint32_t)((i + 1) * segments + j);

			indices[index++] = (uint32_t)(i * segments + j);
			indices[index++] = (uint32_t)(i * segments + j + 1);
			indices[index++] = (uint32_t)((i + 1) * segments + j);
		}
	}

	glGenBuffers(1, &m_IndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_uIndexCount * 4, indices.data(), GL_STATIC_DRAW);
}

void Geometry::GenKnot(uint32_t slices, uint32_t stacks, float radius)
{
	Clear();
	m_eDrawMode = GL_TRIANGLES;

	const size_t vertexCount = slices * stacks;
	m_uIndexCount = vertexCount * 6;

	m_arrVertices.resize(vertexCount);

	const float ds = 1.0f / slices;
	const float dt = 1.0f / stacks;

	// The upper bounds in these loops are tweaked to reduce the
	// chance of precision error causing an incorrect # of iterations.

	auto vertexData = (VERTEX*)m_arrVertices.data();
	for (float s = 0; s < 1 - ds / 2; s += ds)
	{
		for (float t = 0; t < 1 - dt / 2; t += dt)
		{
			const float E = 0.01f;
			const glm::vec3 p = EvaluateTrefoil(s, t);
			const glm::vec3 u = EvaluateTrefoil(s + E, t) - p;
			const glm::vec3 v = EvaluateTrefoil(s, t + E) - p;
			const glm::vec3 n = glm::normalize(glm::cross(v, u));

			vertexData->x = p.x * radius;
			vertexData->y = p.y * radius;
			vertexData->z = p.z * radius;
			vertexData->nx = n.x;
			vertexData->ny = n.y;
			vertexData->nz = n.z;
			vertexData->tu = n.x;
			vertexData->tv = n.y;
			++vertexData;
		}
	}

	std::vector<uint32_t> indices;
	indices.resize(m_uIndexCount);
	uint32_t* indexData = indices.data();

	uint32_t n = 0;
	for (uint32_t i = 0; i < slices; i++)
	{
		for (uint32_t j = 0; j < stacks; j++)
		{
			*indexData++ = (uint32_t)((n + j + stacks) % vertexCount);
			*indexData++ = n + (j + 1) % stacks;
			*indexData++ = n + j;

			*indexData++ = (uint32_t)((n + (j + 1) % stacks + stacks) % vertexCount);
			*indexData++ = (n + (j + 1) % stacks) % vertexCount;
			*indexData++ = (uint32_t)((n + j + stacks) % vertexCount);
		}
		n += (uint32_t)stacks;
	}

	glGenBuffers(1, &m_IndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_uIndexCount * 4, indices.data(), GL_STATIC_DRAW);
}

// not implemented
/*
bool Geometry::LoadObj(const std::string_view& filename)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string warn;
	std::string err;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename.data(), "", true);
	if (!warn.empty())
	{
		IApplication::Debug(warn);
	}
	if (!err.empty())
	{
		IApplication::Debug(err);
	}
	if (!ret)
	{
		return false;
	}

	Clear();

	for (const auto& shape : shapes)
	{
		for (const auto& index : shape.mesh.indices)
		{
			m_arrVertices.emplace_back(VERTEX(
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2],
				attrib.normals[3 * index.normal_index + 0],
				attrib.normals[3 * index.normal_index + 1],
				attrib.normals[3 * index.normal_index + 2],
				attrib.texcoords[3 * index.texcoord_index + 0],
				attrib.texcoords[3 * index.texcoord_index + 1]));
		}
	}

	return true;
}
*/

void Geometry::SetAttribs(GLuint program) const
{
	// get the vertex attribute locations
	const GLint position = glGetAttribLocation(program, "position");
	const GLint normal = glGetAttribLocation(program, "normal");
	const GLint uv = glGetAttribLocation(program, "uv");

	const float* vertexData = (const float*)m_arrVertices.data();

	// set the vertex position
	glEnableVertexAttribArray(position);
	glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, VERTEX::GetStride(), vertexData);

	// vertex normal
	glEnableVertexAttribArray(normal);
	glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, VERTEX::GetStride(), vertexData + 3);

	// vertex uv
	glEnableVertexAttribArray(uv);
	glVertexAttribPointer(uv, 2, GL_FLOAT, GL_FALSE, VERTEX::GetStride(), vertexData + 6);
}

void Geometry::DisableAttribs(GLuint program) const
{
	// get the vertex attribute locations
	const GLint position = glGetAttribLocation(program, "position");
	const GLint normal = glGetAttribLocation(program, "normal");
	const GLint uv = glGetAttribLocation(program, "uv");

	glDisableVertexAttribArray(position);
	glDisableVertexAttribArray(normal);
	glDisableVertexAttribArray(uv);
}

void Geometry::Draw(IRenderer& renderer) const
{
	if (m_IndexBuffer && m_uIndexCount)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
		glDrawElements(m_eDrawMode, (GLsizei)m_uIndexCount, GL_UNSIGNED_INT, 0);
	}
	else
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDrawArrays(m_eDrawMode, 0, (GLsizei)m_arrVertices.size());
	}
}

glm::vec3 Geometry::EvaluateTrefoil(float s, float t)
{
	constexpr float a = 0.5f;
	constexpr float b = 0.3f;
	constexpr float c = 0.5f;
	constexpr float d = 0.1f;
	const float u = (1 - s) * 2 * glm::two_pi<float>();
	const float v = t * glm::two_pi<float>();
	const float r = a + b * cosf(1.5f * u);
	const float x = r * cosf(u);
	const float y = r * sinf(u);
	const float z = c * sinf(1.5f * u);

	glm::vec3 dv;
	dv.x = -1.5f * b * sinf(1.5f * u) * cosf(u) -
		(a + b * cosf(1.5f * u)) * sinf(u);
	dv.y = -1.5f * b * sinf(1.5f * u) * sinf(u) +
		(a + b * cosf(1.5f * u)) * cosf(u);
	dv.z = 1.5f * c * cosf(1.5f * u);

	const glm::vec3 q(glm::normalize(dv));
	const glm::vec3 qvn(glm::normalize(glm::vec3(q.y, -q.x, 0)));
	const glm::vec3 ww = glm::cross(qvn, q);

	glm::vec3 range;
	range.x = x + d * (qvn.x * cosf(v) + ww.x * sinf(v));
	range.y = y + d * (qvn.y * cosf(v) + ww.y * sinf(v));
	range.z = z + d * ww.z * sinf(v);
	return range;
}