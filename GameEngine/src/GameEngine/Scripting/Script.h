#pragma once

#include "GameEngine/Core/Core.h"
#include "GameEngine/Core/Timestep.h"

#include <string>

namespace GameEngine
{
	class Object;

	// Base class for scripts attached to objects
	class GAMEENGINE_API Script
	{
	public:
		virtual ~Script() = default;

		virtual void OnCreate() {}
		virtual void OnUpdate(Timestep deltaTime) {}
		virtual void OnDestroy() {}

		// Script parameters in the Inspector
		virtual void OnImGuiRender() {}

		Object& GetOwner() { return *m_Owner; }
		const std::string& GetName() const { return m_Name; }

	private:
		friend class Object;
		friend class ScriptRegistry;

		Object* m_Owner = nullptr;
		std::string m_Name;
	};
}
