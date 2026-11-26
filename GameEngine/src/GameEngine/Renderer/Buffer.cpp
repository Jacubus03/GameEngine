#include "Buffer.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace GameEngine
{
	VertexBuffer* VertexBuffer::Create(const float* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:     GE_LOG_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:   return new OpenGLVertexBuffer(vertices, size);
		}

		GE_LOG_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	IndexBuffer* IndexBuffer::Create(const uint32_t* indices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:     GE_LOG_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:   return new OpenGLIndexBuffer(indices, size);
		}

		GE_LOG_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	VertexBuffer* VertexBuffer::Create(std::span<const float> vertices)
	{
		return Create(vertices.data(), (uint32_t)vertices.size_bytes());
	}

	IndexBuffer* IndexBuffer::Create(std::span<const uint32_t> indices)
	{
		return Create(indices.data(), (uint32_t)indices.size());
	}
}