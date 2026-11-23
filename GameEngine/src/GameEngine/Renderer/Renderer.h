#pragma once

#include "RenderCommand.h"
#include "RendererAPI.h"
#include "VertexArray.h"
#include "Shader.h"
#include "GameEngine/Scene/Camera/Camera.h"

#include <glm/glm.hpp>

namespace GameEngine
{
	class GAMEENGINE_API Renderer
	{
	public:
		static void Init();

		static void BeginScene(const Ref<Camera>& camera);
		static void EndScene();

		static void Submit(const Ref<VertexArray>& vertexArray, const Ref<Shader>& shader, const glm::mat4& transform = glm::mat4(1.0f));

		static inline RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};

		static SceneData* m_SceneData;
	};
}