#include "PerspectiveCamera.h"

namespace GameEngine
{
    PerspectiveCamera::PerspectiveCamera(float fov, float aspectRatio, float nearClip, float farClip)
        : Camera(glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip))
    {
        m_FOV = fov;
        m_NearClip = nearClip;
        m_FarClip = farClip;
    }
}