#include <GameEngine.h>

#include <algorithm>
#include <imgui.h>
#include <istream>
#include <ostream>

// Moves the object back and forth
class Mover : public GameEngine::Script
{
public:
    void OnUpdate(GameEngine::Timestep deltaTime) override
    {
        if (glm::length(Direction) < 0.0001f)
            return;

        const float step = (std::min)(Speed * (float)deltaTime, Distance - m_Moved);

        GameEngine::Transform& transform = GetOwner().transform;
        transform.SetPosition(transform.GetPosition() + glm::normalize(Direction) * step * m_Sign);

        m_Moved += step;

        if (m_Moved >= Distance)
        {
            m_Moved = 0.0f;
            m_Sign = -m_Sign;
        }
    }

    void OnImGuiRender() override
    {
        ImGui::DragFloat3("Direction", &Direction.x, 0.05f);
        ImGui::DragFloat("Speed", &Speed, 0.05f, 0.0f, 100.0f);
        ImGui::DragFloat("Distance", &Distance, 0.05f, 0.1f, 100.0f);
    }

    void OnSerialize(std::ostream& out) const override
    {
        out << Direction.x << ' ' << Direction.y << ' ' << Direction.z << ' ' << Speed << ' ' << Distance;
    }

    void OnDeserialize(std::istream& in) override
    {
        in >> Direction.x >> Direction.y >> Direction.z >> Speed >> Distance;
    }

public:
    glm::vec3 Direction{ 1.0f, 0.0f, 0.0f };
    float Speed = 1.0f;
    float Distance = 2.0f;

private:
    float m_Moved = 0.0f;
    float m_Sign = 1.0f;
};

GE_REGISTER_SCRIPT(Mover)
