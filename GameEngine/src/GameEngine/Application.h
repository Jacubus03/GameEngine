#pragma once

namespace GameEngine
{
	class __declspec(dllexport) Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	// To be defined in the client application
	Application* CreateApplication();
}