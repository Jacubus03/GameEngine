#pragma once

#include "GameEngine/Renderer/Material.h"
#include "GameEngine/Renderer/Mesh.h"

#include <string>

namespace GameEngine
{
	struct MeshRenderer
	{
		Ref<Mesh> mesh;
		Material material;

		// Where the mesh came from, needed to save the scene
		std::string meshSource;
	};
}
