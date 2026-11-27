#include "WindowsInput.h"

#include "GameEngine/Core/Application.h"
#include <GLFW/glfw3.h>

namespace GameEngine
{
	static bool IsOnAnyMonitor(int x, int y)
	{
		int count = 0;
		GLFWmonitor** monitors = glfwGetMonitors(&count);

		for (int i = 0; i < count; i++)
		{
			int monitorX, monitorY;
			glfwGetMonitorPos(monitors[i], &monitorX, &monitorY);

			const GLFWvidmode* mode = glfwGetVideoMode(monitors[i]);

			if (mode && x >= monitorX && x < monitorX + mode->width && y >= monitorY && y < monitorY + mode->height)
				return true;
		}

		return false;
	}

	Input* Input::s_Instance = new WindowsInput();

	bool WindowsInput::IsKeyPressedImpl(int keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, keycode);

		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool WindowsInput::IsMouseButtonPressedImpl(int button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, button);

		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	std::pair<float, float> WindowsInput::GetMousePositionImpl()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return std::pair<float, float>(static_cast<float>(xpos), static_cast<float>(ypos));
	}

	bool WindowsInput::WrapMouseAtScreenEdgeImpl(float& x, float& y)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());

		int windowX, windowY;
		glfwGetWindowPos(window, &windowX, &windowY);

		double cursorX, cursorY;
		glfwGetCursorPos(window, &cursorX, &cursorY);

		const int screenX = windowX + (int)cursorX;
		const int screenY = windowY + (int)cursorY;

		int count = 0;
		GLFWmonitor** monitors = glfwGetMonitors(&count);

		for (int i = 0; i < count; i++)
		{
			int monitorX, monitorY;
			glfwGetMonitorPos(monitors[i], &monitorX, &monitorY);

			const GLFWvidmode* mode = glfwGetVideoMode(monitors[i]);

			if (!mode || screenX < monitorX || screenX >= monitorX + mode->width ||
				screenY < monitorY || screenY >= monitorY + mode->height)
				continue;

			const int inset = 10;
			int newX = screenX;
			int newY = screenY;

			// Wrap only on the edge of the whole desktop
			if (screenX <= monitorX && !IsOnAnyMonitor(screenX - 1, screenY))
				newX = monitorX + mode->width - inset;
			else if (screenX >= monitorX + mode->width - 1 && !IsOnAnyMonitor(screenX + 1, screenY))
				newX = monitorX + inset;

			if (screenY <= monitorY && !IsOnAnyMonitor(screenX, screenY - 1))
				newY = monitorY + mode->height - inset;
			else if (screenY >= monitorY + mode->height - 1 && !IsOnAnyMonitor(screenX, screenY + 1))
				newY = monitorY + inset;

			if (newX == screenX && newY == screenY)
				return false;

			x = (float)(newX - windowX);
			y = (float)(newY - windowY);
			glfwSetCursorPos(window, x, y);

			return true;
		}

		return false;
	}
}
