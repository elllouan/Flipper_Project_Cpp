#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>
#include <vector>

#if WINDOWS_MSVC
#include <glad/glad.h>
#else
#include <GL/glew.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

    std::string Parse(const std::string &fileName);

public:
    Shader() {}
    ~Shader() = default;

    // TODO: Error handling
    unsigned int CreateShaderProgram(const std::string &vertexShaderFileName, const std::string &fragmentShaderFileName);
    unsigned int Compile(const std::string &fileName, GLenum shaderType);
    void UseProgram();
    void DeleteProgram();
    void SetBool(const std::string &name, bool value, int size = 1) const;
    void SetInt(const std::string &name, int value, int size = 1) const;
    void SetFloat(const std::string &name, float *values, int size = 1) const;
    void SetMatrix4fv(const std::string &name, const float *mat4) const;
    
    unsigned int GetShaderProgram() const;
};


#endif