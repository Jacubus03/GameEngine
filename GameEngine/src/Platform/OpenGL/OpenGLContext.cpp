#include "OpenGLContext.h"

#include "GameEngine/Core.h"
//#include "GameEngine/Log.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace GameEngine
{
    OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
        : m_WindowHandle(windowHandle)
    {
        GE_LOG_CORE_ASSERT(m_WindowHandle, "Window handle is null!");
    }

    void OpenGLContext::Init()
    {
        glfwMakeContextCurrent(m_WindowHandle);
        int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        GE_LOG_CORE_ASSERT(status, "Failed to initialize Glad!");

		GE_LOG_CORE_INFO("OpenGL Info:");
        GE_LOG_CORE_INFO("  Vendor: {0}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
        GE_LOG_CORE_INFO("  Renderer: {0}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
        GE_LOG_CORE_INFO("  Version: {0}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    }

    void OpenGLContext::SwapBuffers()
    {
        glfwSwapBuffers(m_WindowHandle);
    }
}