#pragma once

#include "GameEngine/Core/Core.h"
#include "Texture.h"

#include <glm/glm.hpp>
#include <string>

namespace GameEngine
{
	struct Material
	{
		glm::vec3 Color{ 0.8f, 0.8f, 0.8f };
		float SpecularStrength = 0.5f;
		float Shininess = 32.0f;

		bool UseTexture = true;
		Ref<Texture2D> Texture;
		std::string TexturePath;
	};
}
