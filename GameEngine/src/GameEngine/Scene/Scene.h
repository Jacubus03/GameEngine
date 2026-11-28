#pragma once

#include "Object.h"
#include "GameEngine/Core/Timestep.h"

namespace GameEngine
{
    struct SceneLight
    {
        glm::vec3 Position{ 2.0f, 3.0f, 3.0f };
        glm::vec3 Color{ 1.0f, 1.0f, 1.0f };
        float AmbientStrength = 0.15f;
    };

    class GAMEENGINE_API Scene
    {
    public:
        Scene() = default;
        ~Scene() = default;

        // Copying disabled
        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;

        // Moving enabled
        Scene(Scene&&) noexcept = default;
        Scene& operator=(Scene&&) noexcept = default;

        Object& CreateObject(const std::string& name = "Object");
        void DestroyObject(Object& object);
        void Clear();

        Object* FindObject(const Transform* transform) const;

        // Updates the scripts of all objects
        void OnUpdate(Timestep deltaTime);

        const std::vector<std::unique_ptr<Object>>& GetObjects() const
        {
            return m_Objects;
        }

    public:
        SceneLight light;

    private:
        std::vector<std::unique_ptr<Object>> m_Objects;
    };
}
