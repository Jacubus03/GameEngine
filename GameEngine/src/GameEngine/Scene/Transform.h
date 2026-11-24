
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
        glm::vec3& GetPosition() { return m_Position; }
        const glm::vec3& GetPosition() const { return m_Position; }
        void SetPosition(const glm::vec3& position);

        glm::vec3 GetGlobalPosition() const;
        void SetGlobalPosition(const glm::vec3& position);

        // Rotation (Euler angles in degrees)
        glm::vec3& GetRotation() { return m_Rotation; }
        const glm::vec3& GetRotation() const { return m_Rotation; }
        glm::quat GetLocalRotationQuaternion() const;

        glm::vec3 GetEulerAngles() const { return m_Rotation; }

        void SetRotation(const glm::vec3& angles);
        void SetRotation(float angle, const glm::vec3& axis);

        glm::vec3 GetGlobalRotation() const;
        glm::quat GetGlobalRotationQuaternion() const;

        void SetGlobalRotation(const glm::vec3& angles);
        void SetGlobalRotation(float angle, const glm::vec3& axis);

        // Rotation Helpers
        void Rotate(float angle, const glm::vec3& axis);

        void RotateX(float angle);
        void RotateY(float angle);
        void RotateZ(float angle);

        void RotateGlobal(float angle, const glm::vec3& axis);

        void RotateGlobalX(float angle);
        void RotateGlobalY(float angle);
        void RotateGlobalZ(float angle);

        void RotateXClamped(float angle, float minAngle, float maxAngle);

        // Scale
        glm::vec3& GetScale() { return m_Scale; }
        const glm::vec3& GetScale() const { return m_Scale; }
        void SetScale(const glm::vec3& scale);

        glm::vec3 GetGlobalScale() const;
        void SetGlobalScale(const glm::vec3& scale);

        // Parent
        Transform* GetParent() const { return m_Parent; }
        void SetParent(Transform* parent, bool keepWorldTransform = false);

        const std::vector<Transform*>& GetChildren() const { return m_Children; }

        // Transform Matrices
        glm::mat4 GetTransformMatrix() const;
        glm::mat4 GetGlobalTransformMatrix() const;

        // Callback
        void SetOnTransformChanged(std::function<void()> callback);

    private:
        void RemoveChild(Transform* child);
        void SetTransform(const glm::mat4& transform);
        void NotifyTransformChanged();


    private:
        // Local Transform
        glm::vec3 m_Position{ 0.0f };
        glm::vec3 m_Rotation{ 0.0f };
        glm::vec3 m_Scale{ 1.0f };

        // Hierarchy
        Transform* m_Parent = nullptr;
        std::vector<Transform*> m_Children;

        // Callback
        std::function<void()> m_OnTransformChanged;
    };
}
