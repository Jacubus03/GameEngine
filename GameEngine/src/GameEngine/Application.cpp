#include "Application.h"
#include "GameEngine/Core.h"
#include "GameEngine/Events/Event.h"
#include "GameEngine/Events/ApplicationEvent.h"
#include "GameEngine/Log.h"

namespace GameEngine
{
	Application::Application()
	{
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		WindowResizeEvent resizeEvent(1280, 720);
		LOG_TRACE(resizeEvent.ToString());

		while (true)
		{
			// Update and render the application
		}
	}
}