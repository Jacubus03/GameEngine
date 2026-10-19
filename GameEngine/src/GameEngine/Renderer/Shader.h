#pragma once

#include <string>
#include <unordered_map>

namespace GameEngine
{
    class Shader
    {
    public:
        Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
        ~Shader();

        void Bind() const;
        void Unbind() const;

        void SetInt(const std::string& name, int value);
        void SetFloat(const std::string& name, float value);
        void SetVec2(const std::string& name, const float* value);
        void SetVec3(const std::string& name, const float* value);
        void SetVec4(const std::string& name, const float* value);

    private:
        uint32_t m_RendererID;
    };
}