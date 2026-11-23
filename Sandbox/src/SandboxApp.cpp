#include <GameEngine.h>
#include "GameEngine/Core/EntryPoint.h"
#include "Sandbox3D.h"

#include <Platform/OpenGL/OpenGLShader.h>

class ExampleLayer : public GameEngine::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{
		m_Camera = GameEngine::Ref<GameEngine::Camera>(new GameEngine::PerspectiveCamera(45.0f, 16.0f / 9.0f, 0.1f, 100.0f));
		m_Camera->GetTransform().SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));
		m_CameraController = GameEngine::Ref<GameEngine::SceneCameraController>(new GameEngine::SceneCameraController(*m_Camera));

		// Triangle
		m_TriangleVA.reset(GameEngine::VertexArray::Create());

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f
		};
		GameEngine::Ref<GameEngine::VertexBuffer> vertexBuffer(GameEngine::VertexBuffer::Create(vertices, sizeof(vertices)));

		GameEngine::BufferLayout layout = {
			{ GameEngine::ShaderDataType::Float3, "a_Position" },
			{ GameEngine::ShaderDataType::Float4, "a_Color"},
		};
		vertexBuffer->SetLayout(layout);
		m_TriangleVA->AddVertexBuffer(vertexBuffer);

		unsigned int indices[3] = { 0, 1, 2 };
		GameEngine::Ref<GameEngine::IndexBuffer> indexBuffer(GameEngine::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_TriangleVA->SetIndexBuffer(indexBuffer);

		// Square
		m_SquareVA.reset(GameEngine::VertexArray::Create());

		float vertices1[4 * 5] = {
			-0.75f, -0.75f, 0.0f, 0.0f, 0.0f,
			 0.75f, -0.75f, 0.0f, 1.0f, 0.0f,
			 0.75f,  0.75f, 0.0f, 1.0f, 1.0f,
			 -0.75f,  0.75f, 0.0f, 0.0f, 1.0f
		};

		GameEngine::Ref<GameEngine::VertexBuffer> squareVB(GameEngine::VertexBuffer::Create(vertices1, sizeof(vertices1)));
		GameEngine::BufferLayout layout1 = {
			{ GameEngine::ShaderDataType::Float3, "a_Position" },
			{ GameEngine::ShaderDataType::Float2, "a_TexCoord" },
		};
		squareVB->SetLayout(layout1);
		m_SquareVA->AddVertexBuffer(squareVB);

		unsigned int indices1[6] = { 0, 1, 2, 2, 3, 0 };
		GameEngine::Ref<GameEngine::IndexBuffer> squareIB(GameEngine::IndexBuffer::Create(indices1, sizeof(indices1) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);

		auto vertexColorShader = m_ShaderLibrary.Load("Assets/Shaders/VertexColor.glsl");
		auto flatColorShader = m_ShaderLibrary.Load("Assets/Shaders/FlatColor.glsl");
		auto textureShader = m_ShaderLibrary.Load("Assets/Shaders/Texture.glsl");

		m_ErrorTexture = (GameEngine::Texture2D::Create("Assets/Textures/error_texture.png"));

		std::dynamic_pointer_cast<GameEngine::OpenGLShader>(textureShader)->Bind();
		std::dynamic_pointer_cast<GameEngine::OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 0);
	}

	void OnUpdate(GameEngine::Timestep deltaTime) override
	{
		// Update
		m_CameraController->OnUpdate(deltaTime);

		// Render
		GameEngine::RenderCommand::SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
		GameEngine::RenderCommand::Clear();
		GameEngine::Renderer::BeginScene(m_Camera);

		auto vertexColorShader = m_ShaderLibrary.Get("VertexColor");
		auto flatColorShader = m_ShaderLibrary.Get("FlatColor");
		auto textureShader = m_ShaderLibrary.Get("Texture");

		std::dynamic_pointer_cast<GameEngine::OpenGLShader>(flatColorShader)->Bind();
		std::dynamic_pointer_cast<GameEngine::OpenGLShader>(flatColorShader)->UploadUniformFloat3("u_Color", glm::vec3(0.9f));

		glm::vec3 pos = glm::vec3(0.0f, -0.3f, -1.0f);
		glm::vec3 rot = glm::vec3(0.0f, 0.0f, 45.0f);
		glm::vec3 scale = glm::vec3(1.0f);

		glm::mat4 transform =
			glm::translate(glm::mat4(1.0f), pos) *
			glm::mat4_cast(glm::quat(glm::radians(rot))) *
			glm::scale(glm::mat4(1.0f), scale);

		m_ErrorTexture->Bind();
		GameEngine::Renderer::Submit(m_SquareVA, textureShader, transform);
		GameEngine::Renderer::Submit(m_TriangleVA, vertexColorShader);

		GameEngine::Renderer::EndScene();
	}

	void OnEvent(GameEngine::Event& event) override
	{
		m_CameraController->OnEvent(event);
	}


	GameEngine::ShaderLibrary m_ShaderLibrary;

	GameEngine::Ref<GameEngine::VertexArray> m_TriangleVA;
	GameEngine::Ref<GameEngine::VertexArray> m_SquareVA;

	GameEngine::Ref<GameEngine::Texture2D> m_ErrorTexture;

	GameEngine::Ref<GameEngine::Camera> m_Camera;
	GameEngine::Ref<GameEngine::SceneCameraController> m_CameraController;
};

class Sandbox : public GameEngine::Application
{
	public:
		Sandbox()
		{
			//PushLayer(new ExampleLayer());
			PushLayer(new Sandbox3D());
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