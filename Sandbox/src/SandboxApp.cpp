#include <GameEngine.h>

class ExampleLayer : public GameEngine::Layer
{
public:
	ExampleLayer()
		: Layer("Example") 
	{
		m_Camera = std::shared_ptr<GameEngine::Camera>(new GameEngine::PerspectiveCamera(45.0f, 16.0f / 9.0f, 0.1f, 100.0f));
		m_Camera->SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));
		//m_Camera = std::shared_ptr<GameEngine::Camera>(new GameEngine::OrthographicCamera(-1.6f, 1.6f, -0.9f, 0.9f));

		// Triangle
		m_VertexArray.reset(GameEngine::VertexArray::Create());

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f
		};
		std::shared_ptr<GameEngine::VertexBuffer> vertexBuffer(GameEngine::VertexBuffer::Create(vertices, sizeof(vertices)));

		GameEngine::BufferLayout layout = {
			{ GameEngine::ShaderDataType::Float3, "a_Position" },
			{ GameEngine::ShaderDataType::Float4, "a_Color"},
		};
		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		unsigned int indices[3] = { 0, 1, 2 };
		std::shared_ptr<GameEngine::IndexBuffer> indexBuffer(GameEngine::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		// Square
		m_SquareVA.reset(GameEngine::VertexArray::Create());

		float vertices1[4 * 3] = {
			-0.75f, -0.75f, 0.0f,
			 0.75f, -0.75f, 0.0f,
			 0.75f,  0.75f, 0.0f,
			 -0.75f,  0.75f, 0.0f
		};

		std::shared_ptr<GameEngine::VertexBuffer> squareVB(GameEngine::VertexBuffer::Create(vertices1, sizeof(vertices1)));
		GameEngine::BufferLayout layout1 = {
			{ GameEngine::ShaderDataType::Float3, "a_Position" },
		};
		squareVB->SetLayout(layout1);
		m_SquareVA->AddVertexBuffer(squareVB);

		unsigned int indices1[6] = { 0, 1, 2, 2, 3, 0 };
		std::shared_ptr<GameEngine::IndexBuffer> squareIB(GameEngine::IndexBuffer::Create(indices1, sizeof(indices1) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);


		// Shader
		std::string vertexSrc = R"(
			#version 410 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;
			out vec4 v_Color;
			
			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * u_Transform * vec4(v_Position, 1.0);
			}
		)";

		std::string fragmentSrc = R"(
			#version 410 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			in vec4 v_Color;
			
			void main()
			{
				color = vec4((v_Position + 1.0) / 2.0, 1.0);
				color = v_Color;
			}
		)";

		m_Shader.reset(GameEngine::Shader::Create(vertexSrc, fragmentSrc));


		// Shader
		std::string vertexSrc2 = R"(
			#version 410 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;
			out vec4 v_Color;
			
			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * u_Transform * vec4(v_Position, 1.0);
			}
		)";

		std::string fragmentSrc2 = R"(
			#version 410 core
			
			layout(location = 0) out vec4 color;
			
			void main()
			{
				color = vec4(0.9, 0.9, 0.9, 1.0);
			}
		)";

		m_Shader2.reset(GameEngine::Shader::Create(vertexSrc2, fragmentSrc2));
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

		GameEngine::RenderCommand::Clear();
		GameEngine::Renderer::BeginScene(m_Camera);

		glm::vec3 pos = glm::vec3(0.0f, -0.3f, -1.0f);
		glm::vec3 rot = glm::vec3(0.0f, 0.0f, 45.0f);
		glm::vec3 scale = glm::vec3(1.0f);

		glm::mat4 transform =
			glm::translate(glm::mat4(1.0f), pos) *
			glm::mat4_cast(glm::quat(glm::radians(rot))) *
			glm::scale(glm::mat4(1.0f), scale);

		GameEngine::Renderer::Submit(m_SquareVA, m_Shader2, transform);
		GameEngine::Renderer::Submit(m_VertexArray, m_Shader);

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

	std::shared_ptr<GameEngine::VertexArray> m_VertexArray;
	std::shared_ptr<GameEngine::Shader> m_Shader;

	std::shared_ptr<GameEngine::VertexArray> m_SquareVA;
	std::shared_ptr<GameEngine::Shader> m_Shader2;

	std::shared_ptr<GameEngine::Camera> m_Camera;

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