#pragma once

#include "MeshData.h"
#include "VertexArray.h"

namespace GameEngine
{
	class GAMEENGINE_API Mesh
	{
	public:
		static Ref<Mesh> Create(const MeshData& data);

		const Ref<VertexArray>& GetVertexArray() const { return m_VertexArray; }

		uint32_t GetVertexCount() const { return m_VertexCount; }
		uint32_t GetIndexCount() const { return m_IndexCount; }
		uint32_t GetTriangleCount() const { return m_IndexCount / 3; }
	private:
		Ref<VertexArray> m_VertexArray;
		uint32_t m_VertexCount = 0;
		uint32_t m_IndexCount = 0;
	};
}
