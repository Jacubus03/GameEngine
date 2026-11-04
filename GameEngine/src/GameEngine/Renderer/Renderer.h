#pragma once

#include "RenderCommand.h"
#include "RendererAPI.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Camera/Camera.h"

#include <glm/glm.hpp>

namespace GameEngine
{
	class GAMEENGINE_API Renderer
	{
	public:
		static void BeginScene(const std::shared_ptr<Camera>& camera);
		static void EndScene();

		static void Submit(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader);

		static inline RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};

		static SceneData* m_SceneData;
	};
}