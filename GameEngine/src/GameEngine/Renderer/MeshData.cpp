#include "MeshData.h"

#include <cmath>
#include <numbers>

namespace GameEngine::MeshFactory
{
	MeshData CreateCube()
	{
		MeshData mesh;

		struct Face
		{
			float Normal[3];
			float Corners[4][3];
		};

		const Face faces[6] = {
			{ {  0,  0,  1 }, { { -0.5f, -0.5f,  0.5f }, {  0.5f, -0.5f,  0.5f }, {  0.5f,  0.5f,  0.5f }, { -0.5f,  0.5f,  0.5f } } },
			{ {  0,  0, -1 }, { {  0.5f, -0.5f, -0.5f }, { -0.5f, -0.5f, -0.5f }, { -0.5f,  0.5f, -0.5f }, {  0.5f,  0.5f, -0.5f } } },
			{ {  1,  0,  0 }, { {  0.5f, -0.5f,  0.5f }, {  0.5f, -0.5f, -0.5f }, {  0.5f,  0.5f, -0.5f }, {  0.5f,  0.5f,  0.5f } } },
			{ { -1,  0,  0 }, { { -0.5f, -0.5f, -0.5f }, { -0.5f, -0.5f,  0.5f }, { -0.5f,  0.5f,  0.5f }, { -0.5f,  0.5f, -0.5f } } },
			{ {  0,  1,  0 }, { { -0.5f,  0.5f,  0.5f }, {  0.5f,  0.5f,  0.5f }, {  0.5f,  0.5f, -0.5f }, { -0.5f,  0.5f, -0.5f } } },
			{ {  0, -1,  0 }, { { -0.5f, -0.5f, -0.5f }, {  0.5f, -0.5f, -0.5f }, {  0.5f, -0.5f,  0.5f }, { -0.5f, -0.5f,  0.5f } } },
		};

		const float texCoords[4][2] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

		for (const Face& face : faces)
		{
			const uint32_t first = (uint32_t)mesh.Vertices.size();

			for (int i = 0; i < 4; i++)
			{
				MeshVertex vertex{};

				for (int k = 0; k < 3; k++)
				{
					vertex.Position[k] = face.Corners[i][k];
					vertex.Normal[k] = face.Normal[k];
				}

				vertex.TexCoord[0] = texCoords[i][0];
				vertex.TexCoord[1] = texCoords[i][1];

				mesh.Vertices.push_back(vertex);
			}

			for (uint32_t index : { 0u, 1u, 2u, 2u, 3u, 0u })
				mesh.Indices.push_back(first + index);
		}

		return mesh;
	}

	MeshData CreateUVSphere(uint32_t segments, uint32_t rings)
	{
		MeshData mesh;

		if (segments < 3)
			segments = 3;

		if (rings < 2)
			rings = 2;

		const float pi = std::numbers::pi_v<float>;

		for (uint32_t y = 0; y <= rings; y++)
		{
			const float v = (float)y / rings;
			const float phi = v * pi;

			for (uint32_t x = 0; x <= segments; x++)
			{
				const float u = (float)x / segments;
				const float theta = u * 2.0f * pi;

				const float nx = std::cos(theta) * std::sin(phi);
				const float ny = std::cos(phi);
				const float nz = std::sin(theta) * std::sin(phi);

				mesh.Vertices.push_back({ { nx * 0.5f, ny * 0.5f, nz * 0.5f }, { nx, ny, nz }, { u, 1.0f - v } });
			}
		}

		const uint32_t stride = segments + 1;

		for (uint32_t y = 0; y < rings; y++)
		{
			for (uint32_t x = 0; x < segments; x++)
			{
				const uint32_t a = y * stride + x;
				const uint32_t b = a + stride;

				// Rows at the poles collapse to one point, no triangle there
				if (y != 0)
				{
					for (uint32_t index : { a, a + 1, b })
						mesh.Indices.push_back(index);
				}

				if (y != rings - 1)
				{
					for (uint32_t index : { a + 1, b + 1, b })
						mesh.Indices.push_back(index);
				}
			}
		}

		return mesh;
	}
}
