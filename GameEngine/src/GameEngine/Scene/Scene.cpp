#include "Scene.h"

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
}
