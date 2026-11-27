#include "MeshData.h"

#include <array>
#include <charconv>
#include <cmath>
#include <fstream>
#include <map>
#include <numbers>
#include <sstream>
#include <string_view>

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

	namespace
	{
		bool IsSpace(char c)
		{
			return c == ' ' || c == '\t' || c == '\r';
		}

		std::vector<std::string_view> Split(std::string_view line)
		{
			std::vector<std::string_view> tokens;

			size_t i = 0;
			while (i < line.size())
			{
				while (i < line.size() && IsSpace(line[i]))
					i++;

				size_t start = i;
				while (i < line.size() && !IsSpace(line[i]))
					i++;

				if (i > start)
					tokens.push_back(line.substr(start, i - start));
			}

			return tokens;
		}

		bool ParseFloat(std::string_view text, float& value)
		{
			if (!text.empty() && text[0] == '+')
				text.remove_prefix(1);

			auto result = std::from_chars(text.data(), text.data() + text.size(), value);
			return result.ec == std::errc() && result.ptr == text.data() + text.size();
		}

		bool ParseInt(std::string_view text, int& value)
		{
			if (!text.empty() && text[0] == '+')
				text.remove_prefix(1);

			auto result = std::from_chars(text.data(), text.data() + text.size(), value);
			return result.ec == std::errc() && result.ptr == text.data() + text.size();
		}

		// OBJ indices start at 1, negative ones count from the end
		int ResolveIndex(int index, size_t count)
		{
			if (index > 0)
				index -= 1;
			else if (index < 0)
				index += (int)count;
			else
				return -1;

			return (index >= 0 && (size_t)index < count) ? index : -1;
		}

		void Normalize(float* v)
		{
			const float length = std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);

			if (length > 1e-8f)
			{
				v[0] /= length;
				v[1] /= length;
				v[2] /= length;
			}
		}
	}

	std::optional<MeshData> ParseOBJ(const std::string& text, bool normalize)
	{
		std::vector<std::array<float, 3>> positions;
		std::vector<std::array<float, 3>> normals;
		std::vector<std::array<float, 2>> texCoords;

		MeshData mesh;

		// Vertices with the same v/vt/vn are shared
		std::map<std::array<int, 3>, uint32_t> uniqueVertices;
		std::vector<int> positionIndices;
		std::vector<bool> hasFileNormal;

		std::istringstream stream(text);
		std::string line;

		while (std::getline(stream, line))
		{
			const std::vector<std::string_view> tokens = Split(line);

			if (tokens.empty())
				continue;

			if (tokens[0] == "v")
			{
				std::array<float, 3> position{};

				if (tokens.size() < 4 || !ParseFloat(tokens[1], position[0]) || !ParseFloat(tokens[2], position[1]) || !ParseFloat(tokens[3], position[2]))
					return std::nullopt;

				positions.push_back(position);
			}
			else if (tokens[0] == "vn")
			{
				std::array<float, 3> normal{};

				if (tokens.size() < 4 || !ParseFloat(tokens[1], normal[0]) || !ParseFloat(tokens[2], normal[1]) || !ParseFloat(tokens[3], normal[2]))
					return std::nullopt;

				normals.push_back(normal);
			}
			else if (tokens[0] == "vt")
			{
				std::array<float, 2> texCoord{};

				if (tokens.size() < 2 || !ParseFloat(tokens[1], texCoord[0]))
					return std::nullopt;

				if (tokens.size() > 2 && !ParseFloat(tokens[2], texCoord[1]))
					return std::nullopt;

				texCoords.push_back(texCoord);
			}
			else if (tokens[0] == "f")
			{
				std::vector<uint32_t> face;

				for (size_t t = 1; t < tokens.size(); t++)
				{
					// v, v/vt, v//vn or v/vt/vn
					int indices[3] = { 0, 0, 0 };
					int part = 0;
					size_t start = 0;
					const std::string_view token = tokens[t];

					for (size_t i = 0; i <= token.size(); i++)
					{
						if (i == token.size() || token[i] == '/')
						{
							const std::string_view number = token.substr(start, i - start);

							if (part > 2 || (!number.empty() && !ParseInt(number, indices[part])))
								return std::nullopt;

							part++;
							start = i + 1;
						}
					}

					const int v = ResolveIndex(indices[0], positions.size());
					const int vt = indices[1] ? ResolveIndex(indices[1], texCoords.size()) : -1;
					const int vn = indices[2] ? ResolveIndex(indices[2], normals.size()) : -1;

					if (v < 0 || (indices[1] && vt < 0) || (indices[2] && vn < 0))
						return std::nullopt;

					const std::array<int, 3> key = { v, vt, vn };
					auto it = uniqueVertices.find(key);

					if (it == uniqueVertices.end())
					{
						MeshVertex vertex{};

						for (int k = 0; k < 3; k++)
							vertex.Position[k] = positions[v][k];

						if (vn >= 0)
						{
							for (int k = 0; k < 3; k++)
								vertex.Normal[k] = normals[vn][k];
						}

						if (vt >= 0)
						{
							vertex.TexCoord[0] = texCoords[vt][0];
							vertex.TexCoord[1] = texCoords[vt][1];
						}

						it = uniqueVertices.emplace(key, (uint32_t)mesh.Vertices.size()).first;
						mesh.Vertices.push_back(vertex);
						positionIndices.push_back(v);
						hasFileNormal.push_back(vn >= 0);
					}

					face.push_back(it->second);
				}

				// Triangle fan
				for (size_t i = 1; i + 1 < face.size(); i++)
				{
					mesh.Indices.push_back(face[0]);
					mesh.Indices.push_back(face[i]);
					mesh.Indices.push_back(face[i + 1]);
				}
			}
		}

		if (mesh.Vertices.empty() || mesh.Indices.empty())
			return std::nullopt;

		// Normals for vertices that did not get one from the file
		bool missingNormals = false;
		for (bool has : hasFileNormal)
			missingNormals |= !has;

		if (missingNormals)
		{
			std::vector<std::array<float, 3>> generated(positions.size(), { 0.0f, 0.0f, 0.0f });

			for (size_t i = 0; i + 2 < mesh.Indices.size(); i += 3)
			{
				const MeshVertex& a = mesh.Vertices[mesh.Indices[i]];
				const MeshVertex& b = mesh.Vertices[mesh.Indices[i + 1]];
				const MeshVertex& c = mesh.Vertices[mesh.Indices[i + 2]];

				float e1[3], e2[3], n[3];
				for (int k = 0; k < 3; k++)
				{
					e1[k] = b.Position[k] - a.Position[k];
					e2[k] = c.Position[k] - a.Position[k];
				}

				n[0] = e1[1] * e2[2] - e1[2] * e2[1];
				n[1] = e1[2] * e2[0] - e1[0] * e2[2];
				n[2] = e1[0] * e2[1] - e1[1] * e2[0];

				for (uint32_t index : { mesh.Indices[i], mesh.Indices[i + 1], mesh.Indices[i + 2] })
				{
					for (int k = 0; k < 3; k++)
						generated[positionIndices[index]][k] += n[k];
				}
			}

			for (size_t i = 0; i < mesh.Vertices.size(); i++)
			{
				if (hasFileNormal[i])
					continue;

				float* n = generated[positionIndices[i]].data();
				Normalize(n);

				for (int k = 0; k < 3; k++)
					mesh.Vertices[i].Normal[k] = n[k];
			}
		}

		// Center the mesh and scale it to a unit box
		if (normalize)
		{
			float low[3], high[3];
			for (int k = 0; k < 3; k++)
				low[k] = high[k] = mesh.Vertices[0].Position[k];

			for (const MeshVertex& vertex : mesh.Vertices)
			{
				for (int k = 0; k < 3; k++)
				{
					low[k] = (std::min)(low[k], vertex.Position[k]);
					high[k] = (std::max)(high[k], vertex.Position[k]);
				}
			}

			float size = 0.0f;
			for (int k = 0; k < 3; k++)
				size = (std::max)(size, high[k] - low[k]);

			if (size > 1e-8f)
			{
				for (MeshVertex& vertex : mesh.Vertices)
				{
					for (int k = 0; k < 3; k++)
						vertex.Position[k] = (vertex.Position[k] - (low[k] + high[k]) * 0.5f) / size;
				}
			}
		}

		return mesh;
	}

	std::optional<MeshData> LoadOBJ(const std::string& path, bool normalize)
	{
		std::ifstream file(path, std::ios::in | std::ios::binary);

		if (!file)
			return std::nullopt;

		std::ostringstream buffer;
		buffer << file.rdbuf();

		return ParseOBJ(buffer.str(), normalize);
	}
}
