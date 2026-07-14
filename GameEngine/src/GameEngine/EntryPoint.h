#pragma once

#ifdef GAMEENGINE_PLATFORM_WINDOWS

extern GameEngine::Application* GameEngine::CreateApplication();

int main(int argc, char** argv)
{
	GameEngine::Log::Init();
	LOG_CORE_ERROR("Initialized Log!");
	LOG_INFO("Hello! Var={0}", 5);

	auto app = GameEngine::CreateApplication();
	app->Run();
	delete app;
}

#endif