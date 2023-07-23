#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>
#include <vector>

#if WINDOWS_MSVC
#include <glad/glad.h>
#else
#include <GL/glew.h>
#endif

class Shader
{
private:
    unsigned int m_ID;
    #if WINDOWS_MSVC
    std::string m_filePath = "C:\\Users\\Elouan THEOT\\Documents\\Programming\\c++\\Flipper_Project_Cpp\\shaders\\";
    #else
    std::string m_filePath = "/home/...";
    #endif
    std::string m_sourceCode;
    // std::string m_vertexShader;
    // std::string m_geometryShader;
    std::string m_fragmentShader;
    unsigned int m_program;

    std::string parse(const std::string &fileName);

public:
    Shader() {}
    ~Shader() = default;

    // TODO: Error handling
    unsigned int createShaderProgram(const std::string &vertexShaderFileName, const std::string &fragmentShaderFileName);
    unsigned int compile(const std::string &fileName, GLenum shaderType);
    void useProgram();
    void deleteProgram();
    void setBool(const std::string &name, bool value, int size = 1) const;
    void setInt(const std::string &name, int value, int size = 1) const;
    void setFloat(const std::string &name, float *values, int size = 1) const;

    unsigned int getShaderProgram();
};


#endif