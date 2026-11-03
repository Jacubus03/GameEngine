#pragma once

#include <glm/glm.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace GameEngine
{
    class Camera
    {
    public:
		Camera(const glm::mat4& projection) : m_ProjectionMatrix(projection) { RecalculateViewMatrix(); }
        virtual ~Camera() = default;

        const glm::vec3& GetPosition() const { return m_Position; }
        void SetPosition(const glm::vec3& position) { m_Position = position; RecalculateViewMatrix(); }

        const glm::quat& GetRotation() const { return m_Rotation; }
        const glm::vec3 GetEulerAngles() const { return glm::degrees(glm::eulerAngles(m_Rotation)); }
		void SetRotation(const glm::quat& quaternion) { m_Rotation = glm::normalize(quaternion); RecalculateViewMatrix(); } // Set rotation using a quaternion
		void SetRotation(const glm::vec3& angles) { m_Rotation = glm::quat(glm::radians(angles)); RecalculateViewMatrix(); } // Set rotation using Euler angles (in degrees)
		void SetRotation(float angle, const glm::vec3& axis) { m_Rotation = glm::angleAxis(glm::radians(angle), glm::normalize(axis)); RecalculateViewMatrix(); } // Set rotation using an angle and axis (in degrees)

        const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
        const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
        const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

    public:
        void Rotate(float angle, const glm::vec3& axis);

    protected:
        glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
        glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
        glm::mat4 m_ViewProjectionMatrix = glm::mat4(1.0f);

        glm::vec3 m_Position = glm::vec3(0.0f);
        glm::quat m_Rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

	private:
        void RecalculateViewMatrix();
    };
}