#include <GameEngine.h>

#include <imgui.h>

#include <istream>
#include <ostream>

// Rotates the object around Y axis
class Rotator : public GameEngine::Script
{
public:
    void OnUpdate(GameEngine::Timestep deltaTime) override
    {
        GetOwner().transform.RotateY(Speed * (float)deltaTime);
    }

    void OnImGuiRender() override
    {
        ImGui::DragFloat("Speed", &Speed, 0.5f, -360.0f, 360.0f);
    }

    void OnSerialize(std::ostream& out) const override
    {
        out << Speed;
    }

    void OnDeserialize(std::istream& in) override
    {
        in >> Speed;
    }

public:
    // Degrees per second
    float Speed = 45.0f;
};

GE_REGISTER_SCRIPT(Rotator)
