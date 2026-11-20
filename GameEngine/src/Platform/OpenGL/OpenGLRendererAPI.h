#pragma once

#include "GameEngine/Renderer/RendererAPI.h"
#include "GameEngine/Renderer/VertexArray.h"

namespace GameEngine
{
	class OpenGLRendererAPI : public RendererAPI
	{
		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;
		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray) override;
	};
}