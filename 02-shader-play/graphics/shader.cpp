#include "shader.h"

Shader::Shader(const char *vertexPath, const char *fragmentPath)
{
    vertexShaderPath = vertexPath;
    fragmentShaderPath = fragmentPath;

    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vshaderFile;
    std::ifstream fshaderFile;


    vshaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fshaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        // open source files
        vshaderFile.open(vertexShaderPath);
        fshaderFile.open(fragmentShaderPath);

        // Read file content
        std::stringstream vsstream;
        std::stringstream fsstream;
        vsstream << vshaderFile.rdbuf();
        fsstream << fshaderFile.rdbuf();

        // close source files
        vshaderFile.close();
        fshaderFile.close();

        // load code into strings
        vertexCode = vsstream.str();
        fragmentCode = fsstream.str();
    }
    catch(std::ifstream::failure e)
    {
        std::cout << "SHADER_ERROR: " << e.what() << std::endl;
    }
    
}
