#pragma once

#include <GameEngine.h>

#include "AssetBrowserPanel.h"
#include "ProjectBuilder.h"

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

    void BuildGame();
    void StartPlay();
    void StopPlay();

    void SaveScene(const std::string& path);
    void LoadScene(const std::string& path);
    void OnSceneReplaced();
    GameEngine::Object* FindObjectByTag(const std::string& tag);

    GameEngine::Object* AddMeshObject(const std::string& name, const std::string& source, const glm::vec3& position);

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

    // Play mode, the scene is saved when it starts and restored when it stops
    enum class PlayRequest { None, Start, Stop };

    bool m_Playing = false;
    PlayRequest m_PlayRequest = PlayRequest::None;
    std::string m_PlaySnapshot;
    int m_PlaySelectedIndex = -1;

    // Build and the built game
    bool m_PlayerMode = false;
    glm::vec2 m_PlayerViewport{ 0.0f, 0.0f };
    std::string m_BuildMessage;
    bool m_OpenBuildMessage = false;

    // Scene file
    std::string m_ScenePath = "Assets/Scenes/Scene.scene";
    std::string m_PendingSceneLoad;
    bool m_OpenSaveAs = false;
    char m_SaveAsName[128] = "Scene";

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
