#include "Camera.h"

namespace GameEngine
{
	void Camera::Rotate(float angle, const glm::vec3& axis)
    {
        glm::quat rotation = glm::angleAxis(glm::radians(angle), glm::normalize(axis));
        m_Rotation = glm::normalize(rotation * m_Rotation);
        RecalculateViewMatrix();
    }

	void Camera::RecalculateViewMatrix()
	{
		glm::mat4 transform =
			glm::translate(glm::mat4(1.0f), m_Position) *
			glm::mat4_cast(m_Rotation);

		m_ViewMatrix = glm::inverse(transform);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
}