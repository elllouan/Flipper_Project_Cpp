#include "shader.hpp"

#include <iostream>
#include <fstream>
#include <utility>

/* --------------- Private Functions --------------- */

std::string 
Shader::parse(const std::string &fileName)
{
    std::ifstream file(m_filePath+fileName);
    std::string fileContent;

    if (file)
    {
        std::string line;
        while (std::getline(file, line))
        {
            fileContent += line + "\n";
        }
        file.close();
    }
    else
    {
        std::cerr << "Failed to open file: " << fileName << std::endl;
    }
    return fileContent;
}

/* --------------- Public Functions --------------- */

unsigned int
Shader::createShaderProgram(const std::string &vertexShaderFileName,
    const std::string &fragmentShaderFileName)
{
    m_program = glCreateProgram();
    // m_vertexShader = parse(vertexShaderFileName);
    // m_fragmentShader = parse(m_fragmentShader);
    
    // Compile the vertex shader
    unsigned int vs = compile(vertexShaderFileName, GL_VERTEX_SHADER);
    // Compile the fragment shader
    unsigned int fs = compile(fragmentShaderFileName, GL_FRAGMENT_SHADER);

    // Attach all shaders in one program
    glAttachShader(m_program, vs);
    glAttachShader(m_program, fs);

    // Link the shader program
    glLinkProgram(m_program);

    // Shader Link Error Handling
    int success;
    glGetProgramiv(m_program, GL_LINK_STATUS, &success);
    if(!success) {
        int length;
        glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &length);
        // Alloc dynamically on the stack
        char* message = (char *)alloca(length * sizeof(char));
        glGetProgramInfoLog(m_program, 512, NULL, message);
        std::cout << message << std::endl;
    }

    // Save the current program
    glValidateProgram(m_program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    // Return the current shader program
    return m_program;
}

unsigned int 
Shader::compile(const std::string &fileName, GLenum shaderType)
{
    m_ID = glCreateShader(shaderType);
    m_sourceCode = parse(fileName);
    const char *sourceCode = m_sourceCode.c_str();
    glShaderSource(m_ID, 1, &sourceCode, nullptr);
    glCompileShader(m_ID);

    // Shader Compilation Error Handling
    int result;
    glGetShaderiv(m_ID, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(m_ID, GL_INFO_LOG_LENGTH, &length);
        // Allocate data dynamically on the stack with alloca
        char* message = (char *)alloca(length * sizeof(char));
        glGetShaderInfoLog(m_ID, length, &length, message);
        std::cout << message << std::endl;
        // std::cout << "Compilation Error: " << 
        //     (shaderType == GL_VERTEX_SHADER) ? "GL_VERTEX_SHADER" :
        //     "GL_FRAGMENT_SHADER." << std::endl;
    }

    return m_ID;
}

void
Shader::useProgram()
{
    glUseProgram(m_program);
}

void
Shader::deleteProgram()
{
    glDeleteProgram(m_program);
}

void
Shader::setBool(const std::string &name, bool value, int size) const
{         
    glUniform1i(glGetUniformLocation(m_program, name.c_str()), (int)value); 
}
void
Shader::setInt(const std::string &name, int value, int size) const
{ 
    glUniform1i(glGetUniformLocation(m_program, name.c_str()), value); 
}
void
Shader::setFloat(const std::string &name, float *values, int size) const
{ 
    switch (size)
    {
    case 1:
        glUniform1f(glGetUniformLocation(m_program, name.c_str()), *values);
        break;
    case 2:
        glUniform2f(glGetUniformLocation(m_program, name.c_str()), values[0], values[1]);
        break;
    case 3:
        glUniform3f(glGetUniformLocation(m_program, name.c_str()), values[1], values[2], values[3]);
        break;
    case 4:
        glUniform4f(glGetUniformLocation(m_program, name.c_str()), values[1], values[2], values[3], values[4]);
        break;
    default:
        break;
    } 
} 

/* --------------- Getter & Setter Functions --------------- */

unsigned int 
Shader::getShaderProgram()
{
    return m_program;
}