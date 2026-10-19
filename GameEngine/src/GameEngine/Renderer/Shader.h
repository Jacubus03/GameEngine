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
    private:
        uint32_t m_RendererID;
    };
}