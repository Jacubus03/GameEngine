
#include "Transform.h"

#include <algorithm>
#include <utility>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace GameEngine
{
    namespace
    {
        glm::quat EulerToQuaternion(const glm::vec3& m_Rotation)
        {
            const glm::quat q(glm::radians(glm::vec3(
                m_Rotation.y,
                m_Rotation.x,
                m_Rotation.z
            )));

            return glm::normalize(glm::quat(
                q.w,
                q.y,
                q.x,
                q.z
            ));
        }

        glm::vec3 QuaternionToEuler(const glm::quat& rotation)
        {
            const glm::quat q(
                rotation.w,
                rotation.y,
                rotation.x,
                rotation.z
            );

            return glm::degrees(glm::vec3(
                glm::yaw(q),
                glm::pitch(q),
                glm::roll(q)
            ));
        }
    }

    Transform::Transform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
        : m_Position(position), m_Rotation(rotation), m_Scale(scale)
    {
    }

    Transform::~Transform()
    {
        if (m_Parent)
            m_Parent->RemoveChild(this);

        for (Transform* child : m_Children)
            child->m_Parent = nullptr;
    }

    // Position

    void Transform::SetPosition(const glm::vec3& position)
    {
        m_Position = position;
        NotifyTransformChanged();
    }

    glm::vec3 Transform::GetGlobalPosition() const
    {
        return glm::vec3(GetGlobalTransformMatrix()[3]);
    }

    void Transform::SetGlobalPosition(const glm::vec3& position)
    {
        if (m_Parent)
        {
            const glm::mat4 inverseParent =
                glm::inverse(m_Parent->GetGlobalTransformMatrix());

            m_Position = glm::vec3(
                inverseParent * glm::vec4(position, 1.0f)
            );
        }
        else
        {
            m_Position = position;
        }

        NotifyTransformChanged();
    }

    // Rotation

    glm::quat Transform::GetLocalRotationQuaternion() const
    {
        return EulerToQuaternion(m_Rotation);
    }

    void Transform::SetRotation(const glm::vec3& angles)
    {
        m_Rotation = angles;
        NotifyTransformChanged();
    }

    void Transform::SetRotation(float angle, const glm::vec3& axis)
    {
        Rotate(angle, axis);
    }

    glm::vec3 Transform::GetGlobalRotation() const
    {
        return QuaternionToEuler(GetGlobalRotationQuaternion());
    }

    glm::quat Transform::GetGlobalRotationQuaternion() const
    {
        if (m_Parent)
        {
            return glm::normalize(
                m_Parent->GetGlobalRotationQuaternion() *
                GetLocalRotationQuaternion()
            );
        }

        return GetLocalRotationQuaternion();
    }

    void Transform::SetGlobalRotation(const glm::vec3& angles)
    {
        if (m_Parent)
        {
            const glm::quat parentRotation =
                m_Parent->GetGlobalRotationQuaternion();

            const glm::quat globalRotation =
                EulerToQuaternion(angles);

            const glm::quat localRotation =
                glm::inverse(parentRotation) * globalRotation;

            m_Rotation = QuaternionToEuler(localRotation);
        }
        else
        {
            m_Rotation = angles;
        }

        NotifyTransformChanged();
    }

    void Transform::SetGlobalRotation(float angle, const glm::vec3& axis)
    {
        RotateGlobal(angle, axis);
    }

    // Rotation Helpers

    void Transform::Rotate(float angle, const glm::vec3& axis)
    {
        if (glm::length(axis) < 0.0001f)
            return;

        const glm::quat deltaRotation = glm::angleAxis(
            glm::radians(angle),
            glm::normalize(axis)
        );

        const glm::quat newRotation =
            GetLocalRotationQuaternion() * deltaRotation;

        m_Rotation = QuaternionToEuler(newRotation);

        NotifyTransformChanged();
    }

    void Transform::RotateX(float angle)
    {
        m_Rotation.x += angle;
        NotifyTransformChanged();
    }

    void Transform::RotateY(float angle)
    {
        m_Rotation.y += angle;
        NotifyTransformChanged();
    }

    void Transform::RotateZ(float angle)
    {
        m_Rotation.z += angle;
        NotifyTransformChanged();
    }

    void Transform::RotateGlobal(float angle, const glm::vec3& axis)
    {
        if (glm::length(axis) < 0.0001f)
            return;

        const glm::quat deltaRotation = glm::angleAxis(
            glm::radians(angle),
            glm::normalize(axis)
        );

        const glm::quat newGlobalRotation =
            deltaRotation * GetGlobalRotationQuaternion();

        SetGlobalRotation(QuaternionToEuler(newGlobalRotation));
    }

    void Transform::RotateGlobalX(float angle)
    {
        RotateGlobal(angle, glm::vec3(1.0f, 0.0f, 0.0f));
    }

    void Transform::RotateGlobalY(float angle)
    {
        RotateGlobal(angle, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    void Transform::RotateGlobalZ(float angle)
    {
        RotateGlobal(angle, glm::vec3(0.0f, 0.0f, 1.0f));
    }

    void Transform::RotateXClamped(
        float angle,
        float minAngle,
        float maxAngle)
    {
        m_Rotation.x = glm::clamp(
            m_Rotation.x + angle,
            minAngle,
            maxAngle
        );

        NotifyTransformChanged();
    }

    // Scale

    void Transform::SetScale(const glm::vec3& scale)
    {
        m_Scale = scale;
        NotifyTransformChanged();
    }

    glm::vec3 Transform::GetGlobalScale() const
    {
        if (m_Parent)
            return m_Parent->GetGlobalScale() * m_Scale;

        return m_Scale;
    }

    void Transform::SetGlobalScale(const glm::vec3& scale)
    {
        if (m_Parent)
        {
            const glm::vec3 parentScale = m_Parent->GetGlobalScale();

            m_Scale = scale / parentScale;
        }
        else
        {
            m_Scale = scale;
        }

        NotifyTransformChanged();
    }

    // Parent

    void Transform::SetParent(Transform* parent, bool keepWorldTransform)
    {
        if (parent == m_Parent || parent == this)
            return;

        for (Transform* ancestor = parent; ancestor; ancestor = ancestor->m_Parent)
        {
            if (ancestor == this)
                return;
        }

        glm::mat4 worldTransform;

        if (keepWorldTransform)
            worldTransform = GetGlobalTransformMatrix();

        if (m_Parent)
            m_Parent->RemoveChild(this);

        m_Parent = parent;

        if (m_Parent)
            m_Parent->m_Children.push_back(this);

        if (keepWorldTransform)
        {
            const glm::mat4 localTransform = m_Parent
                ? glm::inverse(m_Parent->GetGlobalTransformMatrix()) * worldTransform
                : worldTransform;

            SetTransform(localTransform);
        }

        NotifyTransformChanged();
    }

    void Transform::RemoveChild(Transform* child)
    {
        auto it = std::find(m_Children.begin(), m_Children.end(), child);

        if (it != m_Children.end())
            m_Children.erase(it);
    }

    // Transform Matrices

    glm::mat4 Transform::GetTransformMatrix() const
    {
        const glm::mat4 translation =
            glm::translate(glm::mat4(1.0f), m_Position);

        const glm::mat4 rotation =
            glm::mat4_cast(GetLocalRotationQuaternion());

        const glm::mat4 scale =
            glm::scale(glm::mat4(1.0f), m_Scale);

        return translation * rotation * scale;
    }

    glm::mat4 Transform::GetGlobalTransformMatrix() const
    {
        if (m_Parent)
            return m_Parent->GetGlobalTransformMatrix() * GetTransformMatrix();

        return GetTransformMatrix();
    }

    void Transform::SetTransform(const glm::mat4& transform)
    {
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::quat rotation;

        glm::decompose(
            transform,
            m_Scale,
            rotation,
            m_Position,
            skew,
            perspective
        );

        m_Rotation = QuaternionToEuler(rotation);
    }

    // Callback

    void Transform::SetOnTransformChanged(std::function<void()> callback)
    {
        m_OnTransformChanged = std::move(callback);
    }

    void Transform::NotifyTransformChanged()
    {
        if (m_OnTransformChanged)
            m_OnTransformChanged();

        for (Transform* child : m_Children)
            child->NotifyTransformChanged();
    }
}
