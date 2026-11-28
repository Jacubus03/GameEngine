#pragma once

#include "Script.h"

#include <concepts>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace GameEngine
{
	// Scripts available by name
	class GAMEENGINE_API ScriptRegistry
	{
	public:
		using Factory = std::function<std::unique_ptr<Script>()>;

		static void Register(const std::string& name, Factory factory);

		template<typename T>
			requires std::derived_from<T, Script> && std::default_initializable<T>
		static void Register(const std::string& name)
		{
			Register(name, []() { return std::make_unique<T>(); });
		}

		// Returns nullptr when the name is not registered
		static std::unique_ptr<Script> Create(const std::string& name);

		static std::vector<std::string> GetNames();
	};
}

// Registers the script when the program starts, use in the .cpp file of the script
#define GE_REGISTER_SCRIPT(Class) \
	static const bool s_##Class##Registered = (::GameEngine::ScriptRegistry::Register<Class>(#Class), true);

