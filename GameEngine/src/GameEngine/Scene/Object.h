#pragma once

#include "Transform.h"
#include "MeshRenderer.h"

#include <memory>
#include <optional>
#include <vector>

namespace GameEngine
{
    class Script;

    class GAMEENGINE_API Object
    {
    public:
        Object(const std::string& name = "Object");
        ~Object();

        // Copying disabled
        Object(const Object&) = delete;
        Object& operator=(const Object&) = delete;

        // Calls OnCreate, returns nullptr when the script is empty
        Script* AddScript(std::unique_ptr<Script> script);
        void RemoveScript(Script* script);
        const std::vector<std::unique_ptr<Script>>& GetScripts() const { return m_Scripts; }

        const std::string& GetName() const { return m_Name; }
        void SetName(const std::string& name) { m_Name = name; }

    public:
        Transform transform;

        // Empty when the object has no mesh
        std::optional<MeshRenderer> meshRenderer;
    private:
        std::string m_Name;
        std::vector<std::unique_ptr<Script>> m_Scripts;
    };
}