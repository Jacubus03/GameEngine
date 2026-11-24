#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace GameEngine
{
    Camera::Camera(const glm::mat4& projection)
        : m_ProjectionMatrix(projection)
    {
        transform.SetOnTransformChanged([this]()
        {
            RecalculateViewMatrix();
        });

        RecalculateViewMatrix();
    }

    void Camera::SetViewportSize(uint32_t width, uint32_t height)
    {
        if (width == 0 || height == 0)
            return;

        m_ViewportWidth = width;
        m_ViewportHeight = height;

        RecalculateProjectionMatrix();
    }

    void Camera::RecalculateViewMatrix()
    {
        m_ViewMatrix = glm::inverse(transform.GetGlobalTransformMatrix());
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

    void Camera::RecalculateProjectionMatrix()
    {
        float aspectRatio = (float)m_ViewportWidth / (float)m_ViewportHeight;

        m_ProjectionMatrix = glm::perspective(
            glm::radians(m_FOV),
            aspectRatio,
            m_NearClip,
            m_FarClip
        );

        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }
}
