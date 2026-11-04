#pragma once

namespace GameEngine
{
	class GAMEENGINE_API GraphicsContext
	{
	public:
		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;
	private:
		void* m_WindowHandle;
	};
}