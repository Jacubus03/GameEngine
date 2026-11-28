#include "SceneSerializer.h"

#include "GameEngine/Core/Log.h"
#include "GameEngine/Renderer/MeshData.h"
#include "GameEngine/Scene/Camera/Camera.h"
#include "GameEngine/Scripting/Script.h"
#include "GameEngine/Scripting/ScriptRegistry.h"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <unordered_map>
#include <vector>

namespace GameEngine
{
	namespace
	{
		const char* s_Header = "GameEngineScene 1";

		void WriteVec3(std::ostream& out, const char* key, const glm::vec3& value)
		{
			out << key << ' ' << value.x << ' ' << value.y << ' ' << value.z << '\n';
		}

		bool ReadVec3(std::istream& in, glm::vec3& value)
		{
			return (bool)(in >> value.x >> value.y >> value.z);
		}
	}

	std::string SceneSerializer::Serialize(const Scene& scene, const Camera* camera)
	{
		std::ostringstream out;
		out << std::setprecision(9);

		out << s_Header << '\n';

		if (camera)
		{
			WriteVec3(out, "CameraPosition", camera->transform.GetPosition());
			WriteVec3(out, "CameraRotation", camera->transform.GetEulerAngles());
		}

		WriteVec3(out, "LightPosition", scene.light.Position);
		WriteVec3(out, "LightColor", scene.light.Color);
		out << "LightAmbient " << scene.light.AmbientStrength << '\n';

		std::unordered_map<const Transform*, int> indices;

		int objectIndex = 0;

		for (const auto& object : scene.GetObjects())
			indices[&object->transform] = objectIndex++;

		for (const auto& object : scene.GetObjects())
		{
			out << "Object\n";
			out << "Name " << object->GetName() << '\n';

			if (!object->tag.empty())
				out << "Tag " << object->tag << '\n';

			if (const Transform* parent = object->transform.GetParent())
			{
				auto it = indices.find(parent);

				if (it != indices.end())
					out << "Parent " << it->second << '\n';
			}

			WriteVec3(out, "Position", object->transform.GetPosition());
			WriteVec3(out, "Rotation", object->transform.GetEulerAngles());
			WriteVec3(out, "Scale", object->transform.GetScale());

			if (object->meshRenderer && !object->meshRenderer->meshSource.empty())
			{
				const Material& material = object->meshRenderer->material;

				out << "Mesh " << object->meshRenderer->meshSource << '\n';
				out << "Material " << material.Color.x << ' ' << material.Color.y << ' ' << material.Color.z << ' '
					<< material.SpecularStrength << ' ' << material.Shininess << ' ' << (material.UseTexture ? 1 : 0) << '\n';

				if (material.Texture && !material.TexturePath.empty())
					out << "Texture " << material.TexturePath << '\n';
			}

			for (const auto& script : object->GetScripts())
			{
				out << "Script " << script->GetName() << '\n';

				std::ostringstream data;
				data << std::setprecision(9);
				script->OnSerialize(data);

				std::string text = data.str();

				for (char& c : text)
				{
					if (c == '\n' || c == '\r')
						c = ' ';
				}

				if (!text.empty())
					out << "ScriptData " << text << '\n';
			}
		}

		return out.str();
	}

	bool SceneSerializer::Deserialize(Scene& scene, const std::string& text, Camera* camera)
	{
		std::istringstream stream(text);
		std::string line;

		if (!std::getline(stream, line) || line.substr(0, line.find('\r')) != s_Header)
		{
			GE_LOG_CORE_ERROR("Scene: wrong file header");
			return false;
		}

		// Built separately so a broken file does not destroy the current scene
		Scene loaded;
		loaded.light = scene.light;

		std::vector<int> parents;
		Object* current = nullptr;
		Script* currentScript = nullptr;

		glm::vec3 cameraPosition(0.0f);
		glm::vec3 cameraRotation(0.0f);
		bool hasCamera = false;

		int lineNumber = 1;

		while (std::getline(stream, line))
		{
			lineNumber++;

			if (!line.empty() && line.back() == '\r')
				line.pop_back();

			if (line.empty())
				continue;

			const size_t space = line.find(' ');
			const std::string key = line.substr(0, space);
			const std::string value = space == std::string::npos ? "" : line.substr(space + 1);

			std::istringstream args(value);
			bool ok = true;

			if (key == "CameraPosition")
			{
				ok = ReadVec3(args, cameraPosition);
				hasCamera = true;
			}
			else if (key == "CameraRotation")
			{
				ok = ReadVec3(args, cameraRotation);
				hasCamera = true;
			}
			else if (key == "LightPosition")
			{
				ok = ReadVec3(args, loaded.light.Position);
			}
			else if (key == "LightColor")
			{
				ok = ReadVec3(args, loaded.light.Color);
			}
			else if (key == "LightAmbient")
			{
				ok = (bool)(args >> loaded.light.AmbientStrength);
			}
			else if (key == "Object")
			{
				current = &loaded.CreateObject();
				currentScript = nullptr;
				parents.push_back(-1);
			}
			else if (!current)
			{
				ok = false;
			}
			else if (key == "Name")
			{
				current->SetName(value);
			}
			else if (key == "Tag")
			{
				current->tag = value;
			}
			else if (key == "Parent")
			{
				ok = (bool)(args >> parents.back());
			}
			else if (key == "Position" || key == "Rotation" || key == "Scale")
			{
				glm::vec3 vector;
				ok = ReadVec3(args, vector);

				if (ok && key == "Position") current->transform.SetPosition(vector);
				if (ok && key == "Rotation") current->transform.SetRotation(vector);
				if (ok && key == "Scale")    current->transform.SetScale(vector);
			}
			else if (key == "Mesh")
			{
				auto data = MeshFactory::CreateFromSource(value);

				if (data)
					current->meshRenderer = MeshRenderer{ Mesh::Create(*data), Material{}, value };
				else
					GE_LOG_CORE_WARN("Scene: could not load mesh {0}", value);
			}
			else if (key == "Material")
			{
				Material material;
				int useTexture = 1;

				ok = (bool)(args >> material.Color.x >> material.Color.y >> material.Color.z
					>> material.SpecularStrength >> material.Shininess >> useTexture);

				if (ok && current->meshRenderer)
				{
					material.UseTexture = useTexture != 0;
					current->meshRenderer->material = material;
				}
			}
			else if (key == "Texture")
			{
				if (current->meshRenderer && std::filesystem::exists(value))
				{
					current->meshRenderer->material.Texture = Texture2D::Create(value);
					current->meshRenderer->material.TexturePath = value;
				}
				else if (current->meshRenderer)
				{
					GE_LOG_CORE_WARN("Scene: texture not found {0}", value);
				}
			}
			else if (key == "Script")
			{
				currentScript = current->AddScript(ScriptRegistry::Create(value));

				if (!currentScript)
					GE_LOG_CORE_WARN("Scene: unknown script {0}", value);
			}
			else if (key == "ScriptData")
			{
				if (currentScript)
				{
					std::istringstream data(value);
					currentScript->OnDeserialize(data);
				}
			}
			else
			{
				GE_LOG_CORE_WARN("Scene: unknown key {0} in line {1}", key, lineNumber);
			}

			if (!ok)
			{
				GE_LOG_CORE_ERROR("Scene: wrong data in line {0}", lineNumber);
				return false;
			}
		}

		const auto& objects = loaded.GetObjects();

		for (size_t i = 0; i < parents.size(); i++)
		{
			if (parents[i] < 0)
				continue;

			if (parents[i] >= (int)objects.size() || parents[i] == (int)i)
			{
				GE_LOG_CORE_ERROR("Scene: wrong parent index {0}", parents[i]);
				return false;
			}

			objects[i]->transform.SetParent(&objects[parents[i]]->transform, false);
		}

		if (camera && hasCamera)
		{
			camera->transform.SetPosition(cameraPosition);
			camera->transform.SetRotation(cameraRotation);
		}

		scene = std::move(loaded);

		return true;
	}

	bool SceneSerializer::SaveToFile(const Scene& scene, const std::string& path, const Camera* camera)
	{
		std::error_code error;
		const std::filesystem::path file(path);

		if (file.has_parent_path())
			std::filesystem::create_directories(file.parent_path(), error);

		std::ofstream out(path, std::ios::out | std::ios::binary);

		if (!out)
			return false;

		out << Serialize(scene, camera);

		return (bool)out;
	}

	bool SceneSerializer::LoadFromFile(Scene& scene, const std::string& path, Camera* camera)
	{
		std::ifstream in(path, std::ios::in | std::ios::binary);

		if (!in)
			return false;

		std::ostringstream buffer;
		buffer << in.rdbuf();

		return Deserialize(scene, buffer.str(), camera);
	}
}
