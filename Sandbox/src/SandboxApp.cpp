#include <GameEngine.h>

class ExampleLayer : public GameEngine::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{

	}

	void OnUpdate() override
	{
		LOG_INFO("ExampleLayer::Update");
	}

	void OnEvent(GameEngine::Event& event) override
	{
		LOG_INFO("ExampleLayer::OnEvent: {0}", event.ToString());
	}
};

class Sandbox : public GameEngine::Application
{
	public:
		Sandbox()
		{
			PushLayer(new ExampleLayer());
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