#include "Mesh.h"

#include <span>

namespace GameEngine
{
	Ref<Mesh> Mesh::Create(const MeshData& data)
	{
		Ref<Mesh> mesh = std::make_shared<Mesh>();

		mesh->m_VertexCount = (uint32_t)data.Vertices.size();
		mesh->m_IndexCount = (uint32_t)data.Indices.size();

		mesh->m_VertexArray.reset(VertexArray::Create());

		const std::span<const float> vertices(
			reinterpret_cast<const float*>(data.Vertices.data()),
			data.Vertices.size() * (sizeof(MeshVertex) / sizeof(float))
		);

		Ref<VertexBuffer> vertexBuffer(VertexBuffer::Create(vertices));

		vertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float3, "a_Normal" },
			{ ShaderDataType::Float2, "a_TexCoord" }
		});

		mesh->m_VertexArray->AddVertexBuffer(vertexBuffer);

		Ref<IndexBuffer> indexBuffer(IndexBuffer::Create(std::span<const uint32_t>(data.Indices)));
		mesh->m_VertexArray->SetIndexBuffer(indexBuffer);

		return mesh;
	}
}
