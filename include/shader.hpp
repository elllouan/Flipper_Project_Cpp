#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>
#include <vector>

#if WINDOW_MSVC
#include <glad/glad.h>
#else
#include <GL/glew.h>
#endif

class Shader
{
private:
    unsigned int m_ID;
    std::string m_filePath = "C:\\Users\\Elouan THEOT\\Documents\\Programming\\c++\\Flipper_Project_Cpp\\shaders\\";
    std::string m_sourceCode;
    unsigned int m_program;

    std::string parse(const std::string &fileName);

public:
    Shader();
    ~Shader() = default;

    // TODO: Error handling
    unsigned int createShaderProgram(const std::string &vertexShaderFileName, const std::string &fragmentShaderFileName);
    void useProgram();
    void deleteProgram();
    unsigned int compile(const std::string &fileName, GLenum shaderType);
    unsigned int getShaderProgram();
};


#endif