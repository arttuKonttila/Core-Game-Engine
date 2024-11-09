/**
 * ============================================================================
 *  Name        : Geometry.h
 *  Part of     : Simple OpenGL graphics engine framework
 *  Description : 3d geometry generator and storage
 *  Version     : 1.00
 *	Author      : Jani Immonen, <realdashdev@gmail.com>
 * ============================================================================
**/

#pragma once

#include <vector>
#include "../include/OpenGLRenderer.h"


class Geometry
{
public:
	struct VERTEX
	{
		VERTEX() :
			x(0.0f), y(0.0f), z(0.0f),
			nx(0.0f), ny(0.0f), nz(0.0f),
			tu(0.0f), tv(0.0f)
		{
		}
		VERTEX(float _x, float _y, float _z,
			float _nx, float _ny, float _nz,
			float _tu, float _tv) :
			x(_x), y(_y), z(_z),
			nx(_nx), ny(_ny), nz(_nz),
			tu(_tu), tv(_tv)
		{
		}
		VERTEX(const glm::vec3& pos, const glm::vec3& normal, float _tu, float _tv) :
			x(pos.x), y(pos.y), z(pos.z),
			nx(normal.x), ny(normal.y), nz(normal.z),
			tu(_tu), tv(_tv)
		{
		}

		/**
		* GetStride
		* @return number of bytes allocated by one vertex
		*/
		static constexpr int GetStride() { return 32; }

		float	x, y, z;
		float	nx, ny, nz;
		float	tu, tv;
	};

	Geometry();
	~Geometry();

	void Clear();

	/**
	 * GenSphere
	 * generate sphere. Sphere is single triangle strip, no indexing
	 * @param radius radius of the sphere
	 * @param offset offset of the vertices
	 * @param rings number of rings in geometry
	 * @param segments number of segments in geometry
	 */
	void GenSphere(const glm::vec3& radius,
		const glm::vec3& offset = glm::vec3(0.0f),
		uint32_t rings = 24,
		uint32_t segments = 24);

	/**
	 * GenCube
	 * generate cube. Cube is a triangle list with index buffer
	 * @param size size of the cube
	 * @param offset offset of the vertices
	 */
	void GenCube(const glm::vec3& size, const glm::vec3& offset = glm::vec3(0.0f));

	/**
	 * GenQuad
	 * generate a front facing quad. Quad is a single triangle list, no indexing
	 * @param size size of the quad
	 * @param offset offset of the vertices
	 */
	void GenQuad(const glm::vec2& size, const glm::vec3& offset = glm::vec3(0.0f));

	/**
	 * GenTorus
	 * generate a torus (donut). Torus is a indexed triangle list
	 * @param segments number of segments in geometry
	 * @param radius radius of the torus
	 * @param fatness fatness of the torus
	 */
	void GenTorus(uint32_t segments, float radius, float fatness);

	/**
	 * GenKnot
	 * generate a trefoil knot. Knot is a indexed triangle list
	 * @param slices number of slices in geometry
	 * @param stacks number of stacks in geometry
	 * @param radius radius of the knot
	 */
	void GenKnot(uint32_t slices, uint32_t stacks, float radius);

	// Not implemented
	//bool LoadObj(const std::string_view& filename);

	void SetAttribs(GLuint program) const;
	void DisableAttribs(GLuint program) const;

	void Draw(IRenderer& renderer) const;

	inline std::vector<Geometry::VERTEX>& GetVertices() { return m_arrVertices; }
	inline const std::vector<Geometry::VERTEX>& GetVertices() const { return m_arrVertices; }
	inline GLuint GetIndexBuffer() const { return m_IndexBuffer; }
	inline size_t GetIndexCount() const { return m_uIndexCount; }

private:
	static glm::vec3 EvaluateTrefoil(float s, float t);

	std::vector<VERTEX>			m_arrVertices;
	GLenum						m_eDrawMode;
	GLuint						m_IndexBuffer;
	size_t						m_uIndexCount;
};

