#include "OrthographicCamera.h"

namespace GameEngine
{
    OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
		: Camera(glm::ortho(left, right, bottom, top, -1.0f, 1.0f))
    {
    }
}