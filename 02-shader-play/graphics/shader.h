#pragma once

#include <stdint.h>
#include <GL/glew.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

enum class ShaderType
{
    VertexShader,
    // GeometryShader,
    FragmentShader,
    // ComputeShader
};

class Shader
{
    public:
        uint32_t ID; // Program ID

        Shader(const char* vertexPath, const char * fragmentPath); // constructor reads and builds shaders
        void use(); // use/ activate the shader
        // utility uniform functions
        void setBool(const std::string &name, bool value) const;
        void setInt(const std::string &name, int value) const;
        void setFloat(const std::string &name, float value) const;

        // reloads the shader source
        void reloadShader(ShaderType type);

    private:
        std::string vertexShaderPath;
        std::string fragmentShaderPath;

};
