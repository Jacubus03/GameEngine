#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace GameEngine
{
    Camera::Camera(const glm::mat4& projection)
        : m_ProjectionMatrix(projection)
    {
        m_Transform.SetOnTransformChanged([this]()
        {
            RecalculateViewMatrix();
        });

        RecalculateViewMatrix();
    }

    void Camera::RecalculateViewMatrix()
    {
        m_ViewMatrix = glm::inverse(m_Transform.GetGlobalTransform());
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }
}
