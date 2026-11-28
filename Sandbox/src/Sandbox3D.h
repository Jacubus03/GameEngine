#pragma once

#include <GameEngine.h>

#include "AssetBrowserPanel.h"

#include <functional>
#include <string>
#include <vector>

class Sandbox3D : public GameEngine::Layer
{
public:
    Sandbox3D();

    void OnAttach() override;
    void OnDetach() override;

    void OnUpdate(GameEngine::Timestep deltaTime) override;
    void OnImGuiRender() override;
    void OnEvent(GameEngine::Event& event) override;

private:
    bool OnWindowDrop(GameEngine::WindowDropEvent& event);

    void BeginDockspace();
    void DrawScenePanel();
    void DrawHierarchyPanel();
    void DrawInspectorPanel();
    void DrawMeshPanel();
    void DrawLightPanel();
    void DrawMaterialInspector(GameEngine::Material& material);
    void DrawScriptsSection(GameEngine::Object& object);

    GameEngine::Object* AddMeshObject(const std::string& name, const GameEngine::MeshData& data, const glm::vec3& position);

    void DrawObjectNode(GameEngine::Object& object);
    void DestroyObjectRecursive(GameEngine::Object& object);

private:
    void HandleAssetDrop(const std::string& path);
    void AcceptAssetDrop();

private:
    GameEngine::ShaderLibrary m_ShaderLibrary;

    GameEngine::Ref<GameEngine::VertexArray> m_TriangleVA;
    GameEngine::Ref<GameEngine::VertexArray> m_SquareVA;

    GameEngine::Ref<GameEngine::Texture2D> m_ErrorTexture;

    GameEngine::Ref<GameEngine::Framebuffer> m_Framebuffer;

    GameEngine::Ref<GameEngine::Camera> m_Camera;
    GameEngine::Ref<GameEngine::SceneCameraController> m_CameraController;

#ifdef SANDBOX_SOURCE_ASSETS_DIR
    AssetBrowserPanel m_AssetBrowser{ "Assets", SANDBOX_SOURCE_ASSETS_DIR };
#else
    AssetBrowserPanel m_AssetBrowser;
#endif

    GameEngine::Scene m_Scene;
    GameEngine::Object* m_Square = nullptr;
    GameEngine::Object* m_Triangle = nullptr;

    int m_MeshView = 2;

    // Light
    glm::vec3 m_LightPosition{ 2.0f, 3.0f, 3.0f };
    glm::vec3 m_LightColor{ 1.0f, 1.0f, 1.0f };
    float m_AmbientStrength = 0.15f;

    // Object currently selected in the Hierarchy panel (nullptr = nothing selected)
    GameEngine::Object* m_SelectedObject = nullptr;

    // Scene window
    glm::vec2 m_SceneSize{ 0.0f, 0.0f };
    bool m_SceneFocused = false;
    bool m_SceneHovered = false;
    bool m_SceneDragging = false;

    // Changes requested by the UI (reparent, create, delete).
    std::vector<std::function<void()>> m_PendingActions;
};
