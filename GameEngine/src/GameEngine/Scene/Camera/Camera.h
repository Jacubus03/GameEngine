#pragma once

#include "GameEngine/Scene/Object.h"

#include <glm/glm.hpp>

namespace GameEngine
{
    class GAMEENGINE_API Camera : public Object
    {
    public:
        Camera(const glm::mat4& projection);
        virtual ~Camera() = default;

        Camera(const Camera&) = delete;
        Camera& operator=(const Camera&) = delete;

        const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
        const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
        const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

        void SetViewportSize(uint32_t width, uint32_t height);

    protected:
        glm::mat4 m_ProjectionMatrix{ 1.0f };
        glm::mat4 m_ViewMatrix{ 1.0f };
        glm::mat4 m_ViewProjectionMatrix{ 1.0f };

        float m_FOV = 45.0f;
        float m_NearClip = 0.1f;
        float m_FarClip = 1000.0f;

        uint32_t m_ViewportWidth = 1280;
        uint32_t m_ViewportHeight = 720;

    private:
        void RecalculateViewMatrix();
        void RecalculateProjectionMatrix();
    };
}
