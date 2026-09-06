#include "Application.h"

namespace GameEngine
{
	Application::Application()
	{
		m_Window = std::unique_ptr<Window>(Window::Create());
		//m_Window->SetEventCallback([](Event& e) {
		//	LOG_TRACE(e.ToString());
		//	});
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		while (m_Running)
		{
			m_Window->OnUpdate();
		}
	}
}