#include "Sandbox3D.h"

#include <imgui.h>
#include "Platform/OpenGL/OpenGLShader.h"

Sandbox3D::Sandbox3D()
    : Layer("Sandbox3D")
{
}

void Sandbox3D::OnAttach()
{
    // Camera
    m_Camera = GameEngine::Ref<GameEngine::Camera>(new GameEngine::PerspectiveCamera(45.0f, 16.0f / 9.0f, 0.1f, 100.0f));
    m_Camera->transform.SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));
    m_CameraController = GameEngine::Ref<GameEngine::SceneCameraController>(new GameEngine::SceneCameraController(*m_Camera));

    m_Square = &m_Scene.CreateObject("Square");
    m_Square->transform.SetPosition({ 0.0f, -0.3f, -1.0f });
    m_Square->transform.SetRotation({ 0.0f, 0.0f, 45.0f });
    m_Square->transform.SetScale({ 1.0f, 1.0f, 1.0f });

    m_Triangle = &m_Scene.CreateObject("Triangle");

    // Triangle
    m_TriangleVA.reset(GameEngine::VertexArray::Create());

    float triangleVertices[] = {
        // Position                 Color
          -0.5f, -0.5f, 0.0f,       1.0f, 0.0f, 0.0f, 1.0f,
           0.5f, -0.5f, 0.0f,       0.0f, 1.0f, 0.0f, 1.0f,
           0.0f,  0.5f, 0.0f,       0.0f, 0.0f, 1.0f, 1.0f
    };

    GameEngine::Ref<GameEngine::VertexBuffer> triangleVB(GameEngine::VertexBuffer::Create(triangleVertices, sizeof(triangleVertices)));

    GameEngine::BufferLayout triangleLayout = {
        { GameEngine::ShaderDataType::Float3, "a_Position" },
        { GameEngine::ShaderDataType::Float4, "a_Color" }
    };

    triangleVB->SetLayout(triangleLayout);
    m_TriangleVA->AddVertexBuffer(triangleVB);

    uint32_t triangleIndices[] = { 0, 1, 2 };

    GameEngine::Ref<GameEngine::IndexBuffer> triangleIB(GameEngine::IndexBuffer::Create(triangleIndices, sizeof(triangleIndices) / sizeof(uint32_t)));

    m_TriangleVA->SetIndexBuffer(triangleIB);

    // Square
    m_SquareVA.reset(GameEngine::VertexArray::Create());

    float squareVertices[] = {
        // Position                   TexCoord
          -0.75f, -0.75f, 0.0f,       0.0f, 0.0f,
           0.75f, -0.75f, 0.0f,       1.0f, 0.0f,
           0.75f,  0.75f, 0.0f,       1.0f, 1.0f,
          -0.75f,  0.75f, 0.0f,       0.0f, 1.0f
    };

    GameEngine::Ref<GameEngine::VertexBuffer> squareVB(
        GameEngine::VertexBuffer::Create(
            squareVertices,
            sizeof(squareVertices)
        )
    );

    GameEngine::BufferLayout squareLayout = {
        { GameEngine::ShaderDataType::Float3, "a_Position" },
        { GameEngine::ShaderDataType::Float2, "a_TexCoord" }
    };

    squareVB->SetLayout(squareLayout);
    m_SquareVA->AddVertexBuffer(squareVB);

    uint32_t squareIndices[] = {
        0, 1, 2,
        2, 3, 0
    };

    GameEngine::Ref<GameEngine::IndexBuffer> squareIB(
        GameEngine::IndexBuffer::Create(
            squareIndices,
            sizeof(squareIndices) / sizeof(uint32_t)
        )
    );

    m_SquareVA->SetIndexBuffer(squareIB);

    // Shaders
    m_ShaderLibrary.Load("Assets/Shaders/VertexColor.glsl");
    m_ShaderLibrary.Load("Assets/Shaders/FlatColor.glsl");

    auto textureShader = m_ShaderLibrary.Load("Assets/Shaders/Texture.glsl");

    // Texture
    m_ErrorTexture = GameEngine::Texture2D::Create("Assets/Textures/error_texture.png");

    auto openGLShader = std::dynamic_pointer_cast<GameEngine::OpenGLShader>(textureShader);

    openGLShader->Bind();
    openGLShader->UploadUniformInt("u_Texture", 0);
}

void Sandbox3D::OnDetach()
{
}


void Sandbox3D::OnUpdate(GameEngine::Timestep deltaTime)
{
    // Update
    m_CameraController->OnUpdate(deltaTime);

    // Render
    GameEngine::RenderCommand::SetClearColor(
        glm::vec4(0.1f, 0.1f, 0.1f, 1.0f)
    );

    GameEngine::RenderCommand::Clear();

    GameEngine::Renderer::BeginScene(m_Camera);

    auto vertexColorShader = m_ShaderLibrary.Get("VertexColor");
    auto flatColorShader = m_ShaderLibrary.Get("FlatColor");
    auto textureShader = m_ShaderLibrary.Get("Texture");

    auto flatColorOpenGL =
        std::dynamic_pointer_cast<GameEngine::OpenGLShader>(flatColorShader);

    flatColorOpenGL->Bind();
    flatColorOpenGL->UploadUniformFloat3("u_Color",glm::vec3(0.9f));

    m_ErrorTexture->Bind();

    GameEngine::Renderer::Submit(m_SquareVA, textureShader, m_Square->transform.GetTransformMatrix());
    GameEngine::Renderer::Submit(m_TriangleVA, vertexColorShader);

    GameEngine::Renderer::EndScene();
}

void Sandbox3D::OnImGuiRender()
{
    ImGui::Begin("Sandbox3D");

    ImGui::Text("Camera");
    ImGui::Separator();

    ImGui::Text("Square");
    ImGui::DragFloat3("Position", &m_Square->transform.GetPosition().x, 0.05f);
    glm::vec3 rotation = glm::eulerAngles(m_Square->transform.GetRotation());
    if (ImGui::DragFloat3("Rotation", &rotation.x, 1.0f))
    {
        m_Square->transform.SetRotation(glm::quat(rotation));
    }
    ImGui::DragFloat3("Scale", &m_Square->transform.GetScale().x, 0.05f);

    ImGui::Separator();
    ImGui::Text("Scene Objects");

    for (const auto& object : m_Scene.GetObjects())
    {
        ImGui::BulletText("%s", object->GetName().c_str());
    }

    ImGui::End();
}

void Sandbox3D::OnEvent(GameEngine::Event& event)
{
    m_CameraController->OnEvent(event);
}
