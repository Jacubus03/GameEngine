#include "Object.h"

#include "GameEngine/Scripting/Script.h"

#include <algorithm>

namespace GameEngine
{
    Object::Object(const std::string& name)
        : m_Name(name)
    {
    }

    Object::~Object()
    {
        for (auto& script : m_Scripts)
            script->OnDestroy();
    }

    Script* Object::AddScript(std::unique_ptr<Script> script)
    {
        if (!script)
            return nullptr;

        script->m_Owner = this;
        m_Scripts.push_back(std::move(script));

        Script* added = m_Scripts.back().get();
        added->OnCreate();

        return added;
    }

    void Object::RemoveScript(Script* script)
    {
        auto it = std::find_if(m_Scripts.begin(), m_Scripts.end(),
            [script](const std::unique_ptr<Script>& current) { return current.get() == script; });

        if (it == m_Scripts.end())
            return;

        (*it)->OnDestroy();
        m_Scripts.erase(it);
    }
}
