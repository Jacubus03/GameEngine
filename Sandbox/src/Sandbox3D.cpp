#include "Sandbox3D.h"

#include <imgui.h>

#include <algorithm>
#include <cstdio>
#include <filesystem>

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
    // Framebuffer
    GameEngine::FramebufferSpecification framebufferSpecification;
    framebufferSpecification.Width = 1280;
    framebufferSpecification.Height = 720;
    m_Framebuffer = GameEngine::Framebuffer::Create(framebufferSpecification);

    // Camera
    m_Camera = GameEngine::Ref<GameEngine::Camera>(new GameEngine::PerspectiveCamera(45.0f, 16.0f / 9.0f, 0.1f, 100.0f));
    m_Camera->transform.SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));
    m_CameraController = GameEngine::Ref<GameEngine::SceneCameraController>(new GameEngine::SceneCameraController(*m_Camera));

    m_Square = &m_Scene.CreateObject("Square");
    m_Square->transform.SetPosition({ 0.0f, -0.3f, -1.0f });
    m_Square->transform.SetRotation({ 0.0f, 0.0f, 45.0f });
    m_Square->transform.SetScale({ 1.0f, 1.0f, 1.0f });
    m_Square->tag = "Square";

    m_Triangle = &m_Scene.CreateObject("Triangle");
    m_Triangle->tag = "Triangle";

    // Triangle
    m_TriangleVA.reset(GameEngine::VertexArray::Create());

    float triangleVertices[] = {
        // Position                 Color
          -0.5f, -0.5f, 0.0f,       1.0f, 0.0f, 0.0f, 1.0f,
           0.5f, -0.5f, 0.0f,       0.0f, 1.0f, 0.0f, 1.0f,
           0.0f,  0.5f, 0.0f,       0.0f, 0.0f, 1.0f, 1.0f
    };

    GameEngine::Ref<GameEngine::VertexBuffer> triangleVB(GameEngine::VertexBuffer::Create(triangleVertices));

    GameEngine::BufferLayout triangleLayout = {
        { GameEngine::ShaderDataType::Float3, "a_Position" },
        { GameEngine::ShaderDataType::Float4, "a_Color" }
    };

    triangleVB->SetLayout(triangleLayout);
    m_TriangleVA->AddVertexBuffer(triangleVB);

    uint32_t triangleIndices[] = { 0, 1, 2 };

    GameEngine::Ref<GameEngine::IndexBuffer> triangleIB(GameEngine::IndexBuffer::Create(triangleIndices));

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

    GameEngine::Ref<GameEngine::VertexBuffer> squareVB(GameEngine::VertexBuffer::Create(squareVertices));

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

    GameEngine::Ref<GameEngine::IndexBuffer> squareIB(GameEngine::IndexBuffer::Create(squareIndices));

    m_SquareVA->SetIndexBuffer(squareIB);

    // Shaders
    m_ShaderLibrary.Load("Assets/Shaders/VertexColor.glsl");
    m_ShaderLibrary.Load("Assets/Shaders/FlatColor.glsl");
    m_ShaderLibrary.Load("Assets/Shaders/MeshDebug.glsl");
    auto blinnPhongShader = m_ShaderLibrary.Load("Assets/Shaders/BlinnPhong.glsl");

    auto textureShader = m_ShaderLibrary.Load("Assets/Shaders/Texture.glsl");

    // Texture
    m_ErrorTexture = GameEngine::Texture2D::Create("Assets/Textures/error_texture.png");

    textureShader->Bind();
    textureShader->SetInt("u_Texture", 0);

    blinnPhongShader->Bind();
    blinnPhongShader->SetInt("u_Texture", 0);

    // Meshes
    AddMeshObject("Cube", "cube", { -1.5f, 0.0f, 0.0f });
    AddMeshObject("Sphere", "sphere", { 1.5f, 0.0f, 0.0f });

    if (GameEngine::Object* torus = AddMeshObject("Torus", "Assets/Models/torus.obj", { 0.0f, 1.0f, -0.5f }))
        torus->transform.SetRotation({ 60.0f, 30.0f, 0.0f });

}

void Sandbox3D::OnDetach()
{
}


void Sandbox3D::OnUpdate(GameEngine::Timestep deltaTime)
{
    // Scene is replaced here, not while the panels are drawn
    if (!m_PendingSceneLoad.empty())
    {
        LoadScene(m_PendingSceneLoad);
        m_PendingSceneLoad.clear();
    }

    // Resize
    const GameEngine::FramebufferSpecification specification = m_Framebuffer->GetSpecification();

    if (m_SceneSize.x > 0.0f && m_SceneSize.y > 0.0f &&
        (specification.Width != (uint32_t)m_SceneSize.x || specification.Height != (uint32_t)m_SceneSize.y))
    {
        m_Framebuffer->Resize((uint32_t)m_SceneSize.x, (uint32_t)m_SceneSize.y);
        m_Camera->SetViewportSize((uint32_t)m_SceneSize.x, (uint32_t)m_SceneSize.y);
    }

    // Update
    if (m_SceneFocused)
        m_CameraController->OnUpdate(deltaTime);

    m_Scene.OnUpdate(deltaTime);

    // Render
    m_Framebuffer->Bind();

    GameEngine::RenderCommand::SetClearColor(
        glm::vec4(0.1f, 0.1f, 0.1f, 1.0f)
    );

    GameEngine::RenderCommand::Clear();

    GameEngine::Renderer::BeginScene(m_Camera);

    auto vertexColorShader = m_ShaderLibrary.Get("VertexColor");
    auto flatColorShader = m_ShaderLibrary.Get("FlatColor");
    auto textureShader = m_ShaderLibrary.Get("Texture");

    flatColorShader->Bind();
    flatColorShader->SetFloat3("u_Color", glm::vec3(0.9f));

    m_ErrorTexture->Bind();

    if (m_Square) GameEngine::Renderer::Submit(m_SquareVA, textureShader, m_Square->transform.GetGlobalTransformMatrix());
    if (m_Triangle) GameEngine::Renderer::Submit(m_TriangleVA, vertexColorShader, m_Triangle->transform.GetGlobalTransformMatrix());

    auto meshShader = m_ShaderLibrary.Get(m_MeshView == 2 ? "BlinnPhong" : "MeshDebug");

    meshShader->Bind();

    if (m_MeshView == 2)
    {
        meshShader->SetFloat3("u_LightPosition", m_Scene.light.Position);
        meshShader->SetFloat3("u_LightColor", m_Scene.light.Color);
        meshShader->SetFloat3("u_CameraPosition", m_Camera->transform.GetGlobalPosition());
        meshShader->SetFloat("u_AmbientStrength", m_Scene.light.AmbientStrength);
    }
    else
    {
        meshShader->SetInt("u_Mode", m_MeshView);
    }

    for (const auto& object : m_Scene.GetObjects())
    {
        if (!object->meshRenderer)
            continue;

        const GameEngine::Material& material = object->meshRenderer->material;

        if (m_MeshView == 2)
        {
            meshShader->SetFloat3("u_Color", material.Color);
            meshShader->SetFloat("u_SpecularStrength", material.SpecularStrength);
            meshShader->SetFloat("u_Shininess", material.Shininess);
            meshShader->SetInt("u_UseTexture", material.UseTexture ? 1 : 0);
        }

        (material.Texture ? material.Texture : m_ErrorTexture)->Bind();

        GameEngine::Renderer::Submit(object->meshRenderer->mesh, meshShader, object->transform.GetGlobalTransformMatrix());
    }

    GameEngine::Renderer::EndScene();

    m_Framebuffer->Unbind();
}

void Sandbox3D::OnImGuiRender()
{
    BeginDockspace();
    DrawScenePanel();
    DrawHierarchyPanel();
    DrawInspectorPanel();
    DrawMeshPanel();
    DrawLightPanel();
    m_AssetBrowser.OnImGuiRender();
}

GameEngine::Object* Sandbox3D::AddMeshObject(const std::string& name, const std::string& source, const glm::vec3& position)
{
    auto data = GameEngine::MeshFactory::CreateFromSource(source);

    if (!data)
    {
        GE_LOG_ERROR("Could not load mesh {0}", source);
        return nullptr;
    }

    GameEngine::Object& object = m_Scene.CreateObject(name);
    object.transform.SetPosition(position);
    object.meshRenderer = GameEngine::MeshRenderer{ GameEngine::Mesh::Create(*data), GameEngine::Material{}, source };

    return &object;
}

void Sandbox3D::HandleAssetDrop(const std::string& path)
{
    const std::filesystem::path file(path);

    std::string extension = file.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c) { return (char)std::tolower(c); });

    if (extension == ".obj")
    {
        // In front of the camera
        const glm::vec3 position = m_Camera->transform.GetGlobalPosition()
            + m_Camera->transform.GetGlobalRotationQuaternion() * glm::vec3(0.0f, 0.0f, -3.0f);

        if (GameEngine::Object* object = AddMeshObject(file.stem().string(), path, position))
            m_SelectedObject = object;
    }
    else if (extension == ".scene")
    {
        m_PendingSceneLoad = path;
    }
    else if (extension == ".cpp")
    {
        const std::string name = file.stem().string();

        if (!m_SelectedObject)
        {
            GE_LOG_WARN("Select an object to add the script");
            return;
        }

        if (!m_SelectedObject->AddScript(GameEngine::ScriptRegistry::Create(name)))
            GE_LOG_WARN("Script {0} is not compiled yet, rebuild the project", name);
    }
    else if (extension == ".png" || extension == ".jpg" || extension == ".jpeg" || extension == ".bmp" || extension == ".tga")
    {
        if (!m_SelectedObject || !m_SelectedObject->meshRenderer)
        {
            GE_LOG_WARN("Select a mesh object to set the texture");
            return;
        }

        GameEngine::Material& material = m_SelectedObject->meshRenderer->material;

        material.Texture = GameEngine::Texture2D::Create(path);
        material.TexturePath = path;
    }
}

void Sandbox3D::AcceptAssetDrop()
{
    if (!ImGui::BeginDragDropTarget())
        return;

    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PATH"))
    {
        // Applied later, objects can change while the panels are drawn
        const std::string path = (const char*)payload->Data;
        m_PendingActions.push_back([this, path]() { HandleAssetDrop(path); });
    }

    ImGui::EndDragDropTarget();
}

void Sandbox3D::DrawLightPanel()
{
    ImGui::Begin("Light");

    GameEngine::SceneLight& light = m_Scene.light;

    ImGui::DragFloat3("Position", &light.Position.x, 0.05f);
    ImGui::ColorEdit3("Color", &light.Color.x);
    ImGui::SliderFloat("Ambient", &light.AmbientStrength, 0.0f, 1.0f);

    ImGui::End();
}

void Sandbox3D::DrawMaterialInspector(GameEngine::Material& material)
{
    ImGui::Separator();
    ImGui::Text("Material");

    ImGui::ColorEdit3("Color", &material.Color.x);
    ImGui::SliderFloat("Specular", &material.SpecularStrength, 0.0f, 1.0f);
    ImGui::SliderFloat("Shininess", &material.Shininess, 1.0f, 256.0f);
    ImGui::Checkbox("Use texture", &material.UseTexture);

    const std::string label = material.Texture ? std::filesystem::path(material.TexturePath).filename().string() : "Default (drop a texture here)";
    ImGui::Button(label.c_str(), ImVec2(-1.0f, 0.0f));

    AcceptAssetDrop();

    if (material.Texture && ImGui::Button("Reset texture"))
    {
        material.Texture = nullptr;
        material.TexturePath.clear();
    }
}

void Sandbox3D::DrawMeshPanel()
{
    ImGui::Begin("Meshes");

    ImGui::RadioButton("Normals", &m_MeshView, 0);
    ImGui::SameLine();
    ImGui::RadioButton("UV", &m_MeshView, 1);
    ImGui::SameLine();
    ImGui::RadioButton("Lit", &m_MeshView, 2);

    ImGui::Separator();

    for (const auto& object : m_Scene.GetObjects())
    {
        if (object->meshRenderer)
        {
            const auto& mesh = object->meshRenderer->mesh;
            ImGui::Text("%s: %u vertices, %u triangles", object->GetName().c_str(), mesh->GetVertexCount(), mesh->GetTriangleCount());
        }
    }

    ImGui::End();
}

void Sandbox3D::DrawScenePanel()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Scene");
    ImGui::PopStyleVar();

    m_SceneFocused = ImGui::IsWindowFocused();
    m_SceneHovered = ImGui::IsWindowHovered();

    // Right button drag started in the scene keeps working outside of the window
    if (m_SceneHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    {
        m_SceneDragging = true;

        // Right click focuses the window too
        ImGui::SetWindowFocus();
        m_SceneFocused = true;
    }

    if (!ImGui::IsMouseDown(ImGuiMouseButton_Right))
        m_SceneDragging = false;

    GameEngine::Application::Get().GetImGuiLayer().BlockEvents(!m_SceneDragging && (!m_SceneFocused || !m_SceneHovered));

    const ImVec2 size = ImGui::GetContentRegionAvail();
    m_SceneSize = { size.x, size.y };

    const uint64_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
    ImGui::Image((ImTextureID)textureID, size, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));

    AcceptAssetDrop();

    ImGui::End();
}

void Sandbox3D::BeginDockspace()
{
    static bool dockspaceOpen = true;

    ImGuiWindowFlags windowFlags =
        ImGuiWindowFlags_MenuBar |
        ImGuiWindowFlags_NoDocking;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    windowFlags |= ImGuiWindowFlags_NoTitleBar;
    windowFlags |= ImGuiWindowFlags_NoCollapse;
    windowFlags |= ImGuiWindowFlags_NoResize;
    windowFlags |= ImGuiWindowFlags_NoMove;
    windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
    windowFlags |= ImGuiWindowFlags_NoNavFocus;

    windowFlags |= ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("Dockspace", &dockspaceOpen, windowFlags);

    ImGui::PopStyleVar(3);

    const ImGuiID dockspaceID = ImGui::GetID("MainDockspace");

    ImGui::DockSpace(
        dockspaceID,
        ImVec2(0.0f, 0.0f),
        ImGuiDockNodeFlags_None
    );

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Save Scene"))
                SaveScene(m_ScenePath);

            if (ImGui::MenuItem("Save Scene As..."))
                m_OpenSaveAs = true;

            if (ImGui::BeginMenu("Open Scene"))
            {
                std::error_code error;

                for (const auto& entry : std::filesystem::directory_iterator("Assets/Scenes", error))
                {
                    if (entry.path().extension() == ".scene" && ImGui::MenuItem(entry.path().stem().string().c_str()))
                        m_PendingSceneLoad = entry.path().generic_string();
                }

                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    if (m_OpenSaveAs)
    {
        ImGui::OpenPopup("Save Scene As");
        m_OpenSaveAs = false;
    }

    if (ImGui::BeginPopupModal("Save Scene As", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::InputText("Name", m_SaveAsName, sizeof(m_SaveAsName));

        if (ImGui::Button("Save") && m_SaveAsName[0] != '\0')
        {
            SaveScene(std::string("Assets/Scenes/") + m_SaveAsName + ".scene");
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel"))
            ImGui::CloseCurrentPopup();

        ImGui::EndPopup();
    }

    ImGui::End();
}

void Sandbox3D::DrawHierarchyPanel()
{
    ImGui::Begin("Hierarchy");

    // Only root objects are drawn here, children are drawn recursively by their parent.
    for (const auto& object : m_Scene.GetObjects())
    {
        if (!object->transform.GetParent())
            DrawObjectNode(*object);
    }

    // Empty area below the tree: click = deselect, drop = unparent, right click = menu.
    const ImVec2 available = ImGui::GetContentRegionAvail();
    if (available.x > 1.0f && available.y > 1.0f)
    {
        ImGui::InvisibleButton("##hierarchy_empty_area", available);

        if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
            m_SelectedObject = nullptr;

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_OBJECT"))
            {
                GameEngine::Object* dragged = *static_cast<GameEngine::Object* const*>(payload->Data);
                m_PendingActions.push_back([dragged]()
                {
                    dragged->transform.SetParent(nullptr, true);
                });
            }
            ImGui::EndDragDropTarget();
        }

        if (ImGui::BeginPopupContextItem("##hierarchy_context"))
        {
            if (ImGui::MenuItem("Create Empty"))
            {
                m_PendingActions.push_back([this]()
                {
                    m_SelectedObject = &m_Scene.CreateObject("Empty");
                });
            }
            ImGui::EndPopup();
        }
    }

    ImGui::End();

    for (auto& action : m_PendingActions)
        action();
    m_PendingActions.clear();
}

void Sandbox3D::DrawObjectNode(GameEngine::Object& object)
{
    using namespace GameEngine;

    const std::vector<Transform*>& children = object.transform.GetChildren();

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

    if (children.empty())
        flags |= ImGuiTreeNodeFlags_Leaf;

    if (m_SelectedObject == &object)
        flags |= ImGuiTreeNodeFlags_Selected;

    const bool open = ImGui::TreeNodeEx(&object, flags, "%s", object.GetName().c_str());

    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
        m_SelectedObject = &object;

    if (ImGui::BeginDragDropSource())
    {
        Object* dragged = &object;
        ImGui::SetDragDropPayload("SCENE_OBJECT", &dragged, sizeof(Object*));
        ImGui::TextUnformatted(object.GetName().c_str());
        ImGui::EndDragDropSource();
    }

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_OBJECT"))
        {
            Object* dragged = *static_cast<Object* const*>(payload->Data);
            Object* newParent = &object;

            m_PendingActions.push_back([dragged, newParent]()
            {
                dragged->transform.SetParent(&newParent->transform, true);
            });
        }
        ImGui::EndDragDropTarget();
    }

    if (ImGui::BeginPopupContextItem())
    {
        Object* target = &object;

        if (ImGui::MenuItem("Add Child"))
        {
            m_PendingActions.push_back([this, target]()
            {
                Object& child = m_Scene.CreateObject("Empty");
                child.transform.SetParent(&target->transform);
                m_SelectedObject = &child;
            });
        }

        if (object.transform.GetParent() && ImGui::MenuItem("Unparent"))
        {
            m_PendingActions.push_back([target]()
            {
                target->transform.SetParent(nullptr, true);
            });
        }

        if (ImGui::MenuItem("Delete"))
        {
            m_PendingActions.push_back([this, target]()
            {
                DestroyObjectRecursive(*target);
            });
        }

        ImGui::EndPopup();
    }

    if (open)
    {
        for (Transform* childTransform : children)
        {
            if (Object* child = m_Scene.FindObject(childTransform))
                DrawObjectNode(*child);
        }

        ImGui::TreePop();
    }
}

void Sandbox3D::DestroyObjectRecursive(GameEngine::Object& object)
{
    const std::vector<GameEngine::Transform*> children = object.transform.GetChildren();

    for (GameEngine::Transform* childTransform : children)
    {
        if (GameEngine::Object* child = m_Scene.FindObject(childTransform))
            DestroyObjectRecursive(*child);
    }

    if (m_SelectedObject == &object) m_SelectedObject = nullptr;
    if (m_Square == &object)         m_Square = nullptr;
    if (m_Triangle == &object)       m_Triangle = nullptr;

    m_Scene.DestroyObject(object);
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

        if (GameEngine::Transform* parentTransform = m_SelectedObject->transform.GetParent())
        {
            if (GameEngine::Object* parent = m_Scene.FindObject(parentTransform))
                ImGui::Text("Parent: %s", parent->GetName().c_str());
        }
        else
        {
            ImGui::TextDisabled("Parent: none");
        }

        ImGui::Separator();
        ImGui::Text("Transform (local)");
        DrawTransformInspector(m_SelectedObject->transform);

        if (m_SelectedObject->meshRenderer)
        {
            const auto& mesh = m_SelectedObject->meshRenderer->mesh;

            ImGui::Separator();
            ImGui::Text("Mesh: %u vertices, %u triangles", mesh->GetVertexCount(), mesh->GetTriangleCount());

            DrawMaterialInspector(m_SelectedObject->meshRenderer->material);
        }

        DrawScriptsSection(*m_SelectedObject);
    }
    else
    {
        ImGui::TextDisabled("No object selected");
    }

    ImGui::End();
}

void Sandbox3D::DrawScriptsSection(GameEngine::Object& object)
{
    ImGui::Separator();
    ImGui::Text("Scripts");

    for (const auto& script : object.GetScripts())
    {
        ImGui::PushID(script.get());

        if (ImGui::CollapsingHeader(script->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
        {
            script->OnImGuiRender();

            GameEngine::Script* target = script.get();
            GameEngine::Object* owner = &object;

            if (ImGui::Button("Remove"))
                m_PendingActions.push_back([owner, target]() { owner->RemoveScript(target); });
        }

        ImGui::PopID();
    }

    if (ImGui::Button("Add Script (or drop a script here)", ImVec2(-1.0f, 0.0f)))
        ImGui::OpenPopup("##add_script");

    AcceptAssetDrop();

    if (ImGui::BeginPopup("##add_script"))
    {
        for (const std::string& name : GameEngine::ScriptRegistry::GetNames())
        {
            if (ImGui::MenuItem(name.c_str()))
            {
                GameEngine::Object* owner = &object;
                m_PendingActions.push_back([owner, name]() { owner->AddScript(GameEngine::ScriptRegistry::Create(name)); });
            }
        }

        ImGui::EndPopup();
    }
}

void Sandbox3D::SaveScene(const std::string& path)
{
    if (!GameEngine::SceneSerializer::SaveToFile(m_Scene, path, m_Camera.get()))
    {
        GE_LOG_ERROR("Could not save scene {0}", path);
        return;
    }

    m_ScenePath = path;

#ifdef SANDBOX_SOURCE_ASSETS_DIR
    // Copy to the project too
    const std::filesystem::path target = std::filesystem::path(SANDBOX_SOURCE_ASSETS_DIR) / std::filesystem::path(path).lexically_relative("Assets");

    std::error_code error;
    std::filesystem::create_directories(target.parent_path(), error);
    std::filesystem::copy_file(path, target, std::filesystem::copy_options::overwrite_existing, error);
#endif

    GE_LOG_INFO("Scene saved: {0}", path);
}

void Sandbox3D::LoadScene(const std::string& path)
{
    if (!GameEngine::SceneSerializer::LoadFromFile(m_Scene, path, m_Camera.get()))
    {
        GE_LOG_ERROR("Could not load scene {0}", path);
        return;
    }

    m_ScenePath = path;
    OnSceneReplaced();

    GE_LOG_INFO("Scene loaded: {0}", path);
}

// Pointers to the old objects are not valid anymore
void Sandbox3D::OnSceneReplaced()
{
    m_SelectedObject = nullptr;
    m_PendingActions.clear();

    m_Square = FindObjectByTag("Square");
    m_Triangle = FindObjectByTag("Triangle");
}

GameEngine::Object* Sandbox3D::FindObjectByTag(const std::string& tag)
{
    for (const auto& object : m_Scene.GetObjects())
    {
        if (object->tag == tag)
            return object.get();
    }

    return nullptr;
}

void Sandbox3D::OnEvent(GameEngine::Event& event)
{
    m_CameraController->OnEvent(event);

    GameEngine::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<GameEngine::WindowDropEvent>(GE_BIND_EVENT_FN(Sandbox3D::OnWindowDrop));
}

bool Sandbox3D::OnWindowDrop(GameEngine::WindowDropEvent& event)
{
    m_AssetBrowser.ImportFiles(event.GetPaths());

    return true;
}
