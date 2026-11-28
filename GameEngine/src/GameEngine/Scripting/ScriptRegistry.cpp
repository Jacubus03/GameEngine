#include "ScriptRegistry.h"

#include <map>

namespace GameEngine
{
	static std::map<std::string, ScriptRegistry::Factory>& GetFactories()
	{
		static std::map<std::string, ScriptRegistry::Factory> factories;
		return factories;
	}

	void ScriptRegistry::Register(const std::string& name, Factory factory)
	{
		GetFactories()[name] = std::move(factory);
	}

	std::unique_ptr<Script> ScriptRegistry::Create(const std::string& name)
	{
		auto it = GetFactories().find(name);

		if (it == GetFactories().end())
			return nullptr;

		std::unique_ptr<Script> script = it->second();
		script->m_Name = name;

		return script;
	}

	std::vector<std::string> ScriptRegistry::GetNames()
	{
		std::vector<std::string> names;

		for (const auto& [name, factory] : GetFactories())
			names.push_back(name);

		return names;
	}
}
