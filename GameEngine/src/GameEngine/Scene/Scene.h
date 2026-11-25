#pragma once

#include "Object.h"

namespace GameEngine
{
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

        Object* FindObject(const Transform* transform) const;

        const std::vector<std::unique_ptr<Object>>& GetObjects() const
        {
            return m_Objects;
        }

    private:
        std::vector<std::unique_ptr<Object>> m_Objects;
    };
}
