#pragma once

#include "GameEngine/Renderer/Material.h"
#include "GameEngine/Renderer/Mesh.h"

namespace GameEngine
{
	struct MeshRenderer
	{
		Ref<Mesh> mesh;
		Material material;
	};
}
