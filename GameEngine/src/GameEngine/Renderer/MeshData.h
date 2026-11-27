#pragma once

#include "GameEngine/Core/Core.h"

#include <cstdint>
#include <vector>

namespace GameEngine
{
	struct MeshVertex
	{
		float Position[3];
		float Normal[3];
		float TexCoord[2];
	};

	struct MeshData
	{
		std::vector<MeshVertex> Vertices;
		std::vector<uint32_t> Indices;
	};

	namespace MeshFactory
	{
		GAMEENGINE_API MeshData CreateCube();
		GAMEENGINE_API MeshData CreateUVSphere(uint32_t segments, uint32_t rings);
	}
}
