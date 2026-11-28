#include "Scene.h"

#include "GameEngine/Scripting/Script.h"

namespace GameEngine
{
    Object& Scene::CreateObject(const std::string& name)
    {
        m_Objects.push_back(std::make_unique<Object>(name));

        return *m_Objects.back();
    }

    void Scene::DestroyObject(Object& object)
    {
        auto it = std::find_if(
            m_Objects.begin(),
            m_Objects.end(),
            [&object](const std::unique_ptr<Object>& current)
            {
                return current.get() == &object;
            }
        );

        if (it != m_Objects.end())
            m_Objects.erase(it);
    }

    void Scene::OnUpdate(Timestep deltaTime)
    {
        for (size_t i = 0; i < m_Objects.size(); i++)
        {
            Object& object = *m_Objects[i];

            for (size_t j = 0; j < object.GetScripts().size(); j++)
                object.GetScripts()[j]->OnUpdate(deltaTime);
        }
    }

    Object* Scene::FindObject(const Transform* transform) const
    {
        for (const auto& object : m_Objects)
        {
            if (&object->transform == transform)
                return object.get();
        }

        return nullptr;
    }
}
