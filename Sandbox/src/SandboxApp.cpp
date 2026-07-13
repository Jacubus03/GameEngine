#include <GameEngine.h>

class Sandbox : public GameEngine::Application
{
	public:
		Sandbox()
		{
			// Initialize your sandbox application here
		}
		~Sandbox()
		{
			// Cleanup your sandbox application here
		}
};

GameEngine::Application* GameEngine::CreateApplication()
{
	return new Sandbox();
}