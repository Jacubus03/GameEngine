#pragma once

#include "Camera.h"

namespace GameEngine
{
	class GAMEENGINE_API PerspectiveCamera : public Camera
	{
	public:
		PerspectiveCamera(float fov, float aspectRatio, float nearClip, float farClip);
	};
}