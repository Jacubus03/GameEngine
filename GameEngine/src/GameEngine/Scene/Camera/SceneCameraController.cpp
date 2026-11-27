#include "SceneCameraController.h"

#include <cmath>

#include "GameEngine/Core/Input.h"
#include "GameEngine/Core/KeyCodes.h"
#include "GameEngine/Core/MouseButtonCodes.h"

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
		glm::quat rotation = m_Camera.transform.GetLocalRotationQuaternion();

		if (Input::IsKeyPressed(GE_KEY_W))
			moveDirection += rotation * glm::vec3(0.0f, 0.0f, -1.0f);

		if (Input::IsKeyPressed(GE_KEY_S))
			moveDirection += rotation * glm::vec3(0.0f, 0.0f, 1.0f);

		if (Input::IsKeyPressed(GE_KEY_A))
			moveDirection += rotation * glm::vec3(-1.0f, 0.0f, 0.0f);

		if (Input::IsKeyPressed(GE_KEY_D))
			moveDirection += rotation * glm::vec3(1.0f, 0.0f, 0.0f);

		// Rotation
		float rotationSpeed = 90.0f * (float)deltaTime;

		if (Input::IsKeyPressed(GE_KEY_Q))
			m_Camera.transform.RotateGlobalY(rotationSpeed);

		if (Input::IsKeyPressed(GE_KEY_E))
			m_Camera.transform.RotateGlobalY(-rotationSpeed);

		// Apply movement
		if (moveDirection != glm::vec3(0.0f))
		{
			moveDirection = glm::normalize(moveDirection);

			glm::vec3 newPosition =
				m_Camera.transform.GetGlobalPosition()
				+ moveDirection * speed * (float)deltaTime;

			m_Camera.transform.SetGlobalPosition(newPosition);
		}
	}

	void SceneCameraController::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);

		dispatcher.Dispatch<MouseMovedEvent>(GE_BIND_EVENT_FN(SceneCameraController::OnMouseMoved));
		dispatcher.Dispatch<MouseScrolledEvent>(GE_BIND_EVENT_FN(SceneCameraController::OnMouseScrolled));
	}

	bool SceneCameraController::OnMouseMoved(MouseMovedEvent& e)
	{
		if (!Input::IsMouseButtonPressed(GE_MOUSE_BUTTON_RIGHT))
		{
			m_FirstMouse = true;
			m_WaitingForWrap = false;
			return false;
		}

		if (m_WaitingForWrap)
		{
			const bool arrived = std::abs(e.GetX() - m_WrapX) <= 1.0f && std::abs(e.GetY() - m_WrapY) <= 1.0f;

			if (arrived || ++m_SkippedEvents > 8)
			{
				m_WaitingForWrap = false;
				m_LastMouseX = e.GetX();
				m_LastMouseY = e.GetY();
			}

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

		m_Camera.transform.RotateGlobalY(xOffset * m_MouseSensitivity);
		m_Camera.transform.RotateXClamped(yOffset * m_MouseSensitivity, -90.0f, 90.0f);

		// Wrap
		float wrapX = e.GetX();
		float wrapY = e.GetY();

		if (Input::WrapMouseAtScreenEdge(wrapX, wrapY))
		{
			m_WrapX = wrapX;
			m_WrapY = wrapY;
			m_WaitingForWrap = true;
			m_SkippedEvents = 0;
		}

		return false;
	}

	bool SceneCameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		return false;
	}
}
