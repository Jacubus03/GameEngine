#pragma once

#include <vector>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace GameEngine
{
    class GAMEENGINE_API Transform
    {
    public:
        Transform() = default;
        Transform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);
        ~Transform();

        Transform(const Transform&) = delete;
        Transform& operator=(const Transform&) = delete;

        // Position
        const glm::vec3& GetPosition() const { return m_Position; }
        void SetPosition(const glm::vec3& position);

        glm::vec3 GetGlobalPosition() const;
        void SetGlobalPosition(const glm::vec3& position);

        // Rotation
        const glm::quat& GetRotation() const { return m_Rotation; }
        glm::vec3 GetEulerAngles() const;

        void SetRotation(const glm::quat& rotation);
        void SetRotation(const glm::vec3& angles);
        void SetRotation(float angle, const glm::vec3& axis);

        glm::quat GetGlobalRotation() const;
        glm::vec3 GetGlobalEulerAngles() const;

        void SetGlobalRotation(const glm::quat& rotation);
        void SetGlobalRotation(const glm::vec3& angles);
        void SetGlobalRotation(float angle, const glm::vec3& axis);

        // Rotation Helpers
        void Rotate(float angle, const glm::vec3& axis);
        void RotateX(float angle) { Rotate(angle, m_Rotation * glm::vec3(1.0f, 0.0f, 0.0f)); }
        void RotateY(float angle) { Rotate(angle, m_Rotation * glm::vec3(0.0f, 1.0f, 0.0f)); }
        void RotateZ(float angle) { Rotate(angle, m_Rotation * glm::vec3(0.0f, 0.0f, 1.0f)); }

        void RotateGlobalX(float angle) { Rotate(angle, glm::vec3(1.0f, 0.0f, 0.0f)); }
        void RotateGlobalY(float angle) { Rotate(angle, glm::vec3(0.0f, 1.0f, 0.0f)); }
        void RotateGlobalZ(float angle) { Rotate(angle, glm::vec3(0.0f, 0.0f, 1.0f)); }

        void RotateXClamped(float angle, float minAngle, float maxAngle);

        // Scale
        const glm::vec3& GetScale() const { return m_Scale; }
        void SetScale(const glm::vec3& scale);

        glm::vec3 GetGlobalScale() const;
        void SetGlobalScale(const glm::vec3& scale);

        // Parent
        Transform* GetParent() const { return m_Parent; }
        void SetParent(Transform* parent, bool keepWorldTransform = false);

        const std::vector<Transform*>& GetChildren() const { return m_Children; }

        // Transform Matrices
        glm::mat4 GetTransform() const;
        glm::mat4 GetGlobalTransform() const;

        // Callback
        void SetOnTransformChanged(std::function<void()> callback);

    private:
        void RemoveChild(Transform* child);
        void SetTransform(const glm::mat4& transform);
        void NotifyTransformChanged();

    private:
        // Local Transform
        glm::vec3 m_Position{ 0.0f };
        glm::quat m_Rotation{ 1.0f, 0.0f, 0.0f, 0.0f };
        glm::vec3 m_Scale{ 1.0f };

        // Hierarchy
        Transform* m_Parent = nullptr;
        std::vector<Transform*> m_Children;

        // Callback
        std::function<void()> m_OnTransformChanged;
    };
}
