#pragma once

#include "GameEngine/Core.h"
#include "GameEngine/Window.h"
#include "GameEngine/Events/Event.h"
#include "GameEngine/Events/ApplicationEvent.h"
#include "GameEngine/Log.h"

namespace GameEngine
{
	class __declspec(dllexport) Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);
	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
	};

	// To be defined in the client application
	Application* CreateApplication();
}