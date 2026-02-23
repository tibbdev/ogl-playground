#pragma once

#include <stdint.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

enum class ShaderType
{
    VertexShader,
    // GeometryShader,
    FragmentShader,
    // ComputeShader
};

struct ShaderData
{
    uint32_t    glid;
    std::string path;
    ShaderType  type;
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
        void setMat4(const std::string &name, glm::mat4 value) const;

        // reloads the shader source
        // void reloadShader(ShaderData &shader);
        // add shader
        // void addShader(std::string path, ShaderType type);
        // ShaderData& getShaderFromPath(std::string path);

    private:
        std::string             vertexShaderPath;
        std::string             fragmentShaderPath;
        std::vector<ShaderData> shaders;

};
