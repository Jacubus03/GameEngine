#include <GameEngine.h>

#include <imgui.h>

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

public:
    // Degrees per second
    float Speed = 45.0f;
};

GE_REGISTER_SCRIPT(Rotator)
