#pragma once

#include "GameEngine/Core/Core.h"

#include <cstdint>
#include <optional>
#include <string>
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

		GAMEENGINE_API std::optional<MeshData> LoadOBJ(const std::string& path, bool normalize = true);
		GAMEENGINE_API std::optional<MeshData> ParseOBJ(const std::string& text, bool normalize = true);
	}
}
