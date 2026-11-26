#pragma once

#include <GameEngine.h>

#include <functional>
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
    void BeginDockspace();
    void DrawScenePanel();
    void DrawHierarchyPanel();
    void DrawInspectorPanel();

    void DrawObjectNode(GameEngine::Object& object);
    void DestroyObjectRecursive(GameEngine::Object& object);

private:
    GameEngine::ShaderLibrary m_ShaderLibrary;

    GameEngine::Ref<GameEngine::VertexArray> m_TriangleVA;
    GameEngine::Ref<GameEngine::VertexArray> m_SquareVA;

    GameEngine::Ref<GameEngine::Texture2D> m_ErrorTexture;

    GameEngine::Ref<GameEngine::Framebuffer> m_Framebuffer;

    GameEngine::Ref<GameEngine::Camera> m_Camera;
    GameEngine::Ref<GameEngine::SceneCameraController> m_CameraController;

    GameEngine::Scene m_Scene;
    GameEngine::Object* m_Square = nullptr;
    GameEngine::Object* m_Triangle = nullptr;

    // Object currently selected in the Hierarchy panel (nullptr = nothing selected)
    GameEngine::Object* m_SelectedObject = nullptr;

    // Scene window
    glm::vec2 m_SceneSize{ 0.0f, 0.0f };
    bool m_SceneFocused = false;
    bool m_SceneHovered = false;

    // Changes requested by the UI (reparent, create, delete).
    std::vector<std::function<void()>> m_PendingActions;
};
