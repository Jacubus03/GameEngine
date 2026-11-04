#pragma once

#include "Camera.h"

namespace GameEngine
{
    class GAMEENGINE_API OrthographicCamera : public Camera
    {
    public:
        OrthographicCamera(float left, float right, float bottom, float top);
    };
}