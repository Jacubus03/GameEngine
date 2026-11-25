#include "Sandbox3D.h"

#include <imgui.h>

#include <cstdio>
#include "Platform/OpenGL/OpenGLShader.h"

namespace
{
    void DrawTransformInspector(GameEngine::Transform& transform)
    {
        // Position
        glm::vec3 position = transform.GetPosition();
        if (ImGui::DragFloat3("Position", &position.x, 0.05f))
            transform.SetPosition(position);

        // Rotation
        glm::vec3 rotation = transform.GetEulerAngles();
        if (ImGui::DragFloat3("Rotation", &rotation.x, 0.5f))
            transform.SetRotation(rotation);

        // Scale
        glm::vec3 scale = transform.GetScale();
        if (ImGui::DragFloat3("Scale", &scale.x, 0.05f))
            transform.SetScale(scale);
    }
}

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
    GameEngine::Renderer::Submit(m_TriangleVA, vertexColorShader, m_Triangle->transform.GetTransformMatrix());

    GameEngine::Renderer::EndScene();
}

void Sandbox3D::OnImGuiRender()
{
    DrawHierarchyPanel();
    DrawInspectorPanel();
}

void Sandbox3D::DrawHierarchyPanel()
{
    ImGui::Begin("Hierarchy");

    for (const auto& object : m_Scene.GetObjects())
    {
        GameEngine::Object* current = object.get();

        // The pointer is used as ID, so objects with the same name do not collide.
        ImGui::PushID(current);

        const bool isSelected = (m_SelectedObject == current);
        if (ImGui::Selectable(current->GetName().c_str(), isSelected))
            m_SelectedObject = current;

        ImGui::PopID();
    }

    // Click on empty space clears the selection.
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered())
        m_SelectedObject = nullptr;

    ImGui::End();
}

void Sandbox3D::DrawInspectorPanel()
{
    ImGui::Begin("Inspector");

    if (m_SelectedObject)
    {
        char nameBuffer[128];
        std::snprintf(nameBuffer, sizeof(nameBuffer), "%s", m_SelectedObject->GetName().c_str());

        if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer)))
            m_SelectedObject->SetName(nameBuffer);

        ImGui::Separator();
        ImGui::Text("Transform");
        DrawTransformInspector(m_SelectedObject->transform);
    }
    else
    {
        ImGui::TextDisabled("No object selected");
    }

    ImGui::End();
}

void Sandbox3D::OnEvent(GameEngine::Event& event)
{
    m_CameraController->OnEvent(event);
}
