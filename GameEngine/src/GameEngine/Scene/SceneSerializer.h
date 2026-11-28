#pragma once

#include "Scene.h"

#include <string>

namespace GameEngine
{
	class Camera;

	// Saves and loads a scene as text
	class GAMEENGINE_API SceneSerializer
	{
	public:
		static std::string Serialize(const Scene& scene, const Camera* camera = nullptr);

		// Returns false and leaves the scene unchanged when the text is not a valid scene
		static bool Deserialize(Scene& scene, const std::string& text, Camera* camera = nullptr);

		static bool SaveToFile(const Scene& scene, const std::string& path, const Camera* camera = nullptr);
		static bool LoadFromFile(Scene& scene, const std::string& path, Camera* camera = nullptr);
	};
}
