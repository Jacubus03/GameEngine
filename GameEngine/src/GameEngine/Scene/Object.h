#pragma once

#include "Transform.h"
#include "MeshRenderer.h"

#include <optional>

namespace GameEngine
{
    class GAMEENGINE_API Object
    {
    public:
        Object(const std::string& name = "Object");
        ~Object() = default;

        const std::string& GetName() const { return m_Name; }
        void SetName(const std::string& name) { m_Name = name; }

    public:
        Transform transform;

        // Empty when the object has no mesh
        std::optional<MeshRenderer> meshRenderer;
    private:
        std::string m_Name;
    };
}