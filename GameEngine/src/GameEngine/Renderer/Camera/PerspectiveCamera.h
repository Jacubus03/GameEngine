#pragma once

#include "Camera.h"

namespace GameEngine
{
	class PerspectiveCamera : public Camera
	{
	public:
		PerspectiveCamera(float fov, float aspectRatio, float nearClip, float farClip);
	};
}