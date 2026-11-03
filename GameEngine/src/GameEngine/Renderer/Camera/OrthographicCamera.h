#pragma once

#include "Camera.h"

namespace GameEngine
{
    class OrthographicCamera : public Camera
    {
    public:
        OrthographicCamera(float left, float right, float bottom, float top);
    };
}