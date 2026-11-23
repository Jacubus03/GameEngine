#pragma once

#include <GameEngine.h>

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
    void InitializeScene();

private:
    GameEngine::ShaderLibrary m_ShaderLibrary;

    GameEngine::Ref<GameEngine::VertexArray> m_TriangleVA;
    GameEngine::Ref<GameEngine::VertexArray> m_SquareVA;

    GameEngine::Ref<GameEngine::Texture2D> m_ErrorTexture;

    GameEngine::Ref<GameEngine::Camera> m_Camera;
    GameEngine::Ref<GameEngine::SceneCameraController> m_CameraController;

    glm::vec3 m_SquarePosition{ 0.0f, -0.3f, -1.0f };
    glm::vec3 m_SquareRotation{ 0.0f, 0.0f, 45.0f };
    glm::vec3 m_SquareScale{ 1.0f };
};
