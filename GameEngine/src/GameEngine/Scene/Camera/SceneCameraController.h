#pragma once

#include "GameEngine/Core/Core.h"

#include "Camera.h"
#include "GameEngine/Core/Timestep.h"
#include "GameEngine/Events/Event.h"
#include "GameEngine/Events/KeyEvent.h"
#include "GameEngine/Events/MouseEvent.h"
#include "GameEngine/Events/ApplicationEvent.h"

namespace GameEngine
{
	class GAMEENGINE_API SceneCameraController
	{
	public:
		SceneCameraController(Camera& camera);

		void OnUpdate(Timestep deltaTime);
		void OnEvent(Event& e);

	private:
		bool OnMouseMoved(MouseMovedEvent& e);
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);

	private:
		Camera& m_Camera;

		float m_MouseSensitivity = 0.1f;
		bool m_FirstMouse = true;

		float m_LastMouseX = 0.0f;
		float m_LastMouseY = 0.0f;
	};
}