#include "SceneCameraController.h"

#include "GameEngine/Input.h"
#include "GameEngine/KeyCodes.h"
#include "GameEngine/MouseButtonCodes.h"

namespace GameEngine
{
	SceneCameraController::SceneCameraController(Camera& camera)
		: m_Camera(camera)
	{
	}

	void SceneCameraController::OnUpdate(Timestep deltaTime)
	{
		float speed = 1.0f;
		glm::vec3 moveDirection(0.0f);

		if (Input::IsKeyPressed(GE_KEY_LEFT_SHIFT))
			speed = 2.5f;

		// Movement
		if (Input::IsKeyPressed(GE_KEY_W))
			moveDirection += m_Camera.GetTransform().GetGlobalRotation() * glm::vec3(0.0f, 0.0f, -1.0f);

		if (Input::IsKeyPressed(GE_KEY_S))
			moveDirection += m_Camera.GetTransform().GetGlobalRotation() * glm::vec3(0.0f, 0.0f, 1.0f);

		if (Input::IsKeyPressed(GE_KEY_A))
			moveDirection += m_Camera.GetTransform().GetGlobalRotation() * glm::vec3(-1.0f, 0.0f, 0.0f);

		if (Input::IsKeyPressed(GE_KEY_D))
			moveDirection += m_Camera.GetTransform().GetGlobalRotation() * glm::vec3(1.0f, 0.0f, 0.0f);

		// Rotation
		float rotationSpeed = 90.0f * (float)deltaTime;

		if (Input::IsKeyPressed(GE_KEY_Q))
			m_Camera.GetTransform().RotateGlobalY(rotationSpeed);

		if (Input::IsKeyPressed(GE_KEY_E))
			m_Camera.GetTransform().RotateGlobalY(-rotationSpeed);

		// Apply movement
		if (moveDirection != glm::vec3(0.0f))
		{
			moveDirection = glm::normalize(moveDirection);

			glm::vec3 newPosition =
				m_Camera.GetTransform().GetGlobalPosition()
				+ moveDirection * speed * (float)deltaTime;

			m_Camera.GetTransform().SetGlobalPosition(newPosition);
		}
	}

	void SceneCameraController::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);

		dispatcher.Dispatch<MouseMovedEvent>(GE_BIND_EVENT_FN(SceneCameraController::OnMouseMoved));
		dispatcher.Dispatch<MouseScrolledEvent>(GE_BIND_EVENT_FN(SceneCameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(GE_BIND_EVENT_FN(SceneCameraController::OnWindowResized));
	}

	bool SceneCameraController::OnMouseMoved(MouseMovedEvent& e)
	{
		if (!Input::IsMouseButtonPressed(GE_MOUSE_BUTTON_RIGHT))
		{
			m_FirstMouse = true;
			return false;
		}

		if (m_FirstMouse)
		{
			m_LastMouseX = e.GetX();
			m_LastMouseY = e.GetY();
			m_FirstMouse = false;
			return false;
		}

		float xOffset = m_LastMouseX - e.GetX();
		float yOffset = m_LastMouseY - e.GetY();

		m_LastMouseX = e.GetX();
		m_LastMouseY = e.GetY();

		glm::vec3 rotation = m_Camera.GetTransform().GetEulerAngles();
		m_Camera.GetTransform().RotateGlobalY(xOffset * m_MouseSensitivity);
		m_Camera.GetTransform().RotateXClamped(yOffset * m_MouseSensitivity, -89.0f, 89.0f);
		GE_LOG_TRACE("X: {0}, Y: {1}, Z: {2}", m_Camera.GetTransform().GetGlobalEulerAngles().x, m_Camera.GetTransform().GetGlobalEulerAngles().y, m_Camera.GetTransform().GetGlobalEulerAngles().z);

		return false;
	}

	bool SceneCameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		return false;
	}

	bool SceneCameraController::OnWindowResized(WindowResizeEvent& e)
	{
		return false;
	}
}