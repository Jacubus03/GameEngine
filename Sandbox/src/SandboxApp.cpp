#include <GameEngine.h>

#include <Platform/OpenGL/OpenGLShader.h>

class ExampleLayer : public GameEngine::Layer
{
public:
	ExampleLayer()
		: Layer("Example") 
	{
		m_Camera = GameEngine::Ref<GameEngine::Camera>(new GameEngine::PerspectiveCamera(45.0f, 16.0f / 9.0f, 0.1f, 100.0f));
		m_Camera->SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));
		//m_Camera = GameEngine::Ref<GameEngine::Camera>(new GameEngine::OrthographicCamera(-1.6f, 1.6f, -0.9f, 0.9f));

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

		m_VertexColorShader.reset(GameEngine::Shader::Create("Assets/Shaders/VertexColor.glsl"));
		m_FlatColorShader.reset(GameEngine::Shader::Create("Assets/Shaders/FlatColor.glsl"));
		m_TextureShader.reset(GameEngine::Shader::Create("Assets/Shaders/Texture.glsl"));

		m_ErrorTexture = (GameEngine::Texture2D::Create("Assets/Textures/error_texture.png"));

		std::dynamic_pointer_cast<GameEngine::OpenGLShader>(m_TextureShader)->Bind();
		std::dynamic_pointer_cast<GameEngine::OpenGLShader>(m_TextureShader)->UploadUniformInt("u_Texture", 0);
	}

	void OnUpdate(GameEngine::Timestep deltaTime) override
	{
		float speed = 1.0f;
		glm::vec3 moveDirection = glm::vec3();
		if (GameEngine::Input::IsKeyPressed(GE_KEY_LEFT_SHIFT))
		{
			speed = 2.5f;
		}
		if (GameEngine::Input::IsKeyPressed(GE_KEY_W))
		{
			moveDirection += m_Camera->GetRotation() * glm::vec3(0.0f, 0.0f, -0.1f);
		}
		if (GameEngine::Input::IsKeyPressed(GE_KEY_S))
		{
			moveDirection += m_Camera->GetRotation() * glm::vec3(0.0f, 0.0f, 0.1f);
		}
		if (GameEngine::Input::IsKeyPressed(GE_KEY_A))
		{
			moveDirection += m_Camera->GetRotation() * glm::vec3(-0.1f, 0.0f, 0.0f);
		}
		if (GameEngine::Input::IsKeyPressed(GE_KEY_D))
		{
			moveDirection += m_Camera->GetRotation() * glm::vec3(0.1f, 0.0f, 0.0f);
		}
		if (GameEngine::Input::IsKeyPressed(GE_KEY_Q))
		{
			m_Camera->RotateY(5.0f);
		}
		if (GameEngine::Input::IsKeyPressed(GE_KEY_E))
		{
			m_Camera->RotateY(-5.0f);
		}
		if (moveDirection != glm::vec3()) m_Camera->SetPosition(m_Camera->GetPosition() + glm::normalize(moveDirection) * speed * (float)deltaTime);

		GameEngine::RenderCommand::SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
		GameEngine::RenderCommand::Clear();
		GameEngine::Renderer::BeginScene(m_Camera);

		std::dynamic_pointer_cast<GameEngine::OpenGLShader>(m_FlatColorShader)->Bind();
		std::dynamic_pointer_cast<GameEngine::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", glm::vec3(0.9f));

		glm::vec3 pos = glm::vec3(0.0f, -0.3f, -1.0f);
		glm::vec3 rot = glm::vec3(0.0f, 0.0f, 45.0f);
		glm::vec3 scale = glm::vec3(1.0f);

		glm::mat4 transform =
			glm::translate(glm::mat4(1.0f), pos) *
			glm::mat4_cast(glm::quat(glm::radians(rot))) *
			glm::scale(glm::mat4(1.0f), scale);

		m_ErrorTexture->Bind();
		GameEngine::Renderer::Submit(m_SquareVA, m_TextureShader, transform);
		GameEngine::Renderer::Submit(m_TriangleVA, m_VertexColorShader);

		GameEngine::Renderer::EndScene();
	}

	void OnEvent(GameEngine::Event& event) override
	{
		GameEngine::EventDispatcher dispatcher(event);

		dispatcher.Dispatch<GameEngine::MouseMovedEvent>(GE_BIND_EVENT_FN(ExampleLayer::OnMouseMovedEvent));
	}

	bool OnMouseMovedEvent(GameEngine::MouseMovedEvent& e)
	{
		if (!GameEngine::Input::IsMouseButtonPressed(GE_MOUSE_BUTTON_RIGHT))
		{
			m_FirstMouse = true;
			return false;
		}

		if (m_FirstMouse)
		{
			m_LastMouseX = e.GetX();
			m_LastMouseY = e.GetY();
			m_FirstMouse = false;
			return false;
		}

		float xOffset = m_LastMouseX - e.GetX();
		float yOffset = m_LastMouseY - e.GetY();

		m_LastMouseX = e.GetX();
		m_LastMouseY = e.GetY();

		constexpr float sensitivity = 0.1f;

		m_Camera->RotateY(xOffset * sensitivity);
		m_Camera->RotateLocalX(yOffset * sensitivity);

		return false;
	}

	GameEngine::Ref<GameEngine::VertexArray> m_TriangleVA;
	GameEngine::Ref<GameEngine::VertexArray> m_SquareVA;

	GameEngine::Ref<GameEngine::Shader> m_VertexColorShader;
	GameEngine::Ref<GameEngine::Shader> m_FlatColorShader;
	GameEngine::Ref<GameEngine::Shader> m_TextureShader;

	GameEngine::Ref<GameEngine::Texture2D> m_ErrorTexture;

	GameEngine::Ref<GameEngine::Camera> m_Camera;

	bool m_FirstMouse = true;
	float m_LastMouseX = 0.0f;
	float m_LastMouseY = 0.0f;
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