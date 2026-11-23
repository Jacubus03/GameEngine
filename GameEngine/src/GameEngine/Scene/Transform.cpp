#include "Transform.h"

#include <algorithm>
#include <utility>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace GameEngine
{
    Transform::Transform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
        : m_Position(position), m_Rotation(glm::quat(glm::radians(rotation))), m_Scale(scale)
    {
    }

    Transform::~Transform()
    {
        if (m_Parent)
            m_Parent->RemoveChild(this);

        for (Transform* child : m_Children)
            child->m_Parent = nullptr;
    }

    // Local Position
    void Transform::SetPosition(const glm::vec3& position)
    {
        m_Position = position;
        NotifyTransformChanged();
    }

    // Global Position
    glm::vec3 Transform::GetGlobalPosition() const
    {
        return glm::vec3(GetGlobalTransform()[3]);
    }

    void Transform::SetGlobalPosition(const glm::vec3& position)
    {
        if (m_Parent)
        {
            glm::mat4 inverseParent = glm::inverse(m_Parent->GetGlobalTransform());
            m_Position = glm::vec3(inverseParent * glm::vec4(position, 1.0f));
        }
        else
        {
            m_Position = position;
        }

        NotifyTransformChanged();
    }

    // Local Rotation
    glm::vec3 Transform::GetEulerAngles() const
    {
        return glm::degrees(glm::eulerAngles(m_Rotation));
    }

    void Transform::SetRotation(const glm::quat& rotation)
    {
        m_Rotation = glm::normalize(rotation);
        NotifyTransformChanged();
    }

    void Transform::SetRotation(const glm::vec3& angles)
    {
        m_Rotation = glm::quat(glm::radians(angles));
        NotifyTransformChanged();
    }

    void Transform::SetRotation(float angle, const glm::vec3& axis)
    {
        m_Rotation = glm::angleAxis(glm::radians(angle), glm::normalize(axis));
        NotifyTransformChanged();
    }

    // Global Rotation
    glm::quat Transform::GetGlobalRotation() const
    {
        if (m_Parent)
            return glm::normalize(m_Parent->GetGlobalRotation() * m_Rotation);

        return m_Rotation;
    }

    glm::vec3 Transform::GetGlobalEulerAngles() const
    {
        return glm::degrees(glm::eulerAngles(GetGlobalRotation()));
    }

    void Transform::SetGlobalRotation(const glm::quat& rotation)
    {
        if (m_Parent)
            m_Rotation = glm::normalize(glm::inverse(m_Parent->GetGlobalRotation()) * rotation);
        else
            m_Rotation = glm::normalize(rotation);

        NotifyTransformChanged();
    }

    void Transform::SetGlobalRotation(const glm::vec3& angles)
    {
        SetGlobalRotation(glm::quat(glm::radians(angles)));
    }

    void Transform::SetGlobalRotation(float angle, const glm::vec3& axis)
    {
        SetGlobalRotation(glm::angleAxis(glm::radians(angle), glm::normalize(axis)));
    }

    // Rotation Helpers
    void Transform::Rotate(float angle, const glm::vec3& axis)
    {
        glm::quat rotation = glm::angleAxis(glm::radians(angle), glm::normalize(axis));
        m_Rotation = glm::normalize(rotation * m_Rotation);

        NotifyTransformChanged();
    }

    // Local Scale
    void Transform::SetScale(const glm::vec3& scale)
    {
        m_Scale = scale;
        NotifyTransformChanged();
    }

    // Global Scale
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
            glm::vec3 parentScale = m_Parent->GetGlobalScale();
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

        // Prevent circular hierarchy
        for (Transform* ancestor = parent; ancestor; ancestor = ancestor->m_Parent)
        {
            if (ancestor == this)
                return;
        }

        glm::mat4 worldTransform;

        if (keepWorldTransform)
            worldTransform = GetGlobalTransform();

        if (m_Parent)
            m_Parent->RemoveChild(this);

        m_Parent = parent;

        if (m_Parent)
            m_Parent->m_Children.push_back(this);

        if (keepWorldTransform)
        {
            glm::mat4 localTransform = m_Parent
                ? glm::inverse(m_Parent->GetGlobalTransform()) * worldTransform
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
    glm::mat4 Transform::GetTransform() const
    {
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), m_Position);
        glm::mat4 rotation = glm::mat4_cast(m_Rotation);
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), m_Scale);

        return translation * rotation * scale;
    }

    glm::mat4 Transform::GetGlobalTransform() const
    {
        if (m_Parent)
            return m_Parent->GetGlobalTransform() * GetTransform();

        return GetTransform();
    }

    void Transform::SetTransform(const glm::mat4& transform)
    {
        glm::vec3 skew;
        glm::vec4 perspective;

        glm::decompose(transform, m_Scale, m_Rotation, m_Position, skew, perspective);
        m_Rotation = glm::normalize(m_Rotation);
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
