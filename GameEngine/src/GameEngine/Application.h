#pragma once

#include "GameEngine/Core.h"
#include "GameEngine/Window.h"
#include "GameEngine/LayerStack.h"
#include "GameEngine/Events/Event.h"
#include "GameEngine/Events/ApplicationEvent.h"
#include "GameEngine/Log.h"
#include "GameEngine/Core/Timestep.h"
#include "GameEngine/ImGui/ImGuiLayer.h"
#include "GameEngine/Renderer/Shader.h"
#include "GameEngine/Renderer/Buffer.h"
#include "GameEngine/Renderer/VertexArray.h"
#include "GameEngine/Renderer/Renderer.h"
#include "GameEngine/Renderer/Camera/Camera.h"
#include "GameEngine/Renderer/Camera/OrthographicCamera.h"
#include "GameEngine/Renderer/Camera/PerspectiveCamera.h"

namespace GameEngine
{
	class GAMEENGINE_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		inline Window& GetWindow() { return *m_Window; }

		inline static Application& Get() { return *s_Instance; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);
	private:
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;
	private:
		static Application* s_Instance;
	};

	// To be defined in the client application
	Application* CreateApplication();
}