#include <iostream>
#include <fstream>
#include <string>

#include "shader.hpp"

// turns the header to a .cpp
#define STB_IMAGE_IMPLEMENTATION
// load a image loader lib
#include "stb_image.h"

#if IMGUI
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#endif

// Either GLAD or GLEW
#if WINDOWS_MSVC
#include <glad/glad.h>
#else
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h> // Will drag system OpenGL headers

// Include for GLM headers
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

static std::string parseShaderFile(const std::string& fileName)
{
    std::string path = "C:\\Users\\Elouan THEOT\\Documents\\Programming\\c++\\Flipper_Project_Cpp\\shaders\\";
    std::ifstream file(path+fileName);
    std::string fileContent;

    if (file)
    {
        std::string line;
        while (std::getline(file, line))
        {
            fileContent += line + "\n";
        }
    }
    else
    {
        std::cerr << "Failed to open file: " << fileName << std::endl;
    }
    return fileContent;
}

/*
 * @brief Compile the given shader.
 * @param shaderType Any type of shader (preferably use macros).
 * @param source Source code of the shader.
 * @return The shader's ID (unsigned int).
*/
static unsigned int CompileShader(unsigned int shaderType,
    const std::string& source)
{
    unsigned int shaderId = glCreateShader(shaderType);
    const char* shaderSrc = source.c_str();
    glShaderSource(shaderId, 1, &shaderSrc, nullptr);
    glCompileShader(shaderId);

    // Shader Compilation Error Handling
    int result;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length);
        // Allocate data dynamically on the stack with alloca
        char* message = (char *)alloca(length * sizeof(char));
        glGetShaderInfoLog(shaderId, length, &length, message);
        std::cout << message << std::endl;
    }
    return shaderId;
}

/*
 * @brief Create (Compile/Attach/Link) the given shaders sources and a program.
 * @return A shader program (unsigned int).
*/
static unsigned int CreateShader(const std::string& vertexShader,
    const std::string& fragmentShader)
{
    unsigned int shaderProgram = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    glLinkProgram(shaderProgram);

    // Shader Link Error Handling
    int success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        int length;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &length);
        char* message = (char *)alloca(length * sizeof(char));
        glGetProgramInfoLog(shaderProgram, 512, NULL, message);
        std::cout << message << std::endl;
    }

    glValidateProgram(shaderProgram);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return shaderProgram;
}

/*
 * @brief Create a complete VBO and VAO1, ready to be rendered out.
 * This will call all necessary functions to generate, bind, attribute and populate both VBO and VAO1.
 * (Could be better if pass structs as arguments)
 * @return Void
*/
static void CreateVertexAttribObject(
    unsigned int &VBO,
    unsigned int &VAO,
    GLenum target,
    const void *dataArray, 
    int nbData,
    GLenum usage,
    unsigned int index,
    int nbAttributes,
    GLenum type,
    const void *startPointer
    )
{
    glBindVertexArray(VAO);
    
    glBindBuffer(target, VBO);
    glBufferData(target, nbData*sizeof(type), dataArray, usage);

    glVertexAttribPointer(index, nbAttributes, type, GL_FALSE, nbAttributes*sizeof(type), startPointer);
    glEnableVertexAttribArray(index);
}

int main()
{
    glfwInit();
    const char* glsl_version = "#version 330";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

#if WINDOWS_MSVC
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
#else
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }
#endif
    // Print out the current version
    std::cout << glGetString(GL_VERSION) << std::endl;

    glViewport(0,0,1000,500);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

#if IMGUI
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
#endif

    float rectangles[] = {
        /*  positions   |   colors    | textures  */
        -0.25, -0.25, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0,
        -0.25,  0.25, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0,
         0.25,  0.25, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0,
         0.25, -0.25, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0,
    };
    unsigned int indices[] = {
        0, 1, 2,
        0, 2, 3,
    };

    // Loading the data
    unsigned int VBO1, VAO1, EBO1;
    glGenVertexArrays(1, &VAO1);
    glGenBuffers(1, &VBO1);
    glGenBuffers(1, &EBO1);

    glBindVertexArray(VAO1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectangles), rectangles, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO1);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void *)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void *)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int woodTexture, smileyTexture;
    glGenTextures(1, &woodTexture);

    // glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, woodTexture);

    // Wrapping methods
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Filtering methods
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char *data = stbi_load("C:\\Users\\Elouan THEOT\\Documents\\Programming\\c++\\Flipper_Project_Cpp\\img\\container.jpg", 
        &width, &height, &nrChannels, 0);

    if(data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to generate a 2D texture image.\n";
    }
    stbi_image_free(data);


    glGenTextures(1, &smileyTexture);
    glBindTexture(GL_TEXTURE_2D, smileyTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    data = stbi_load("C:\\Users\\Elouan THEOT\\Documents\\Programming\\c++\\Flipper_Project_Cpp\\img\\smiley.jpg",
        &width, &height, &nrChannels, 0);

    if(data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to generate a 2D texture image.\n";
    }
    stbi_image_free(data);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Create the shader program
    Shader myShader = Shader();
    unsigned int prog = myShader.createShaderProgram("vertexShaderRec.vs", "fragmentShaderRec.fs");

    myShader.useProgram();
    // The second argument refers to the active texture
    myShader.setInt("texture1", 0); // texture1 in the vertex shader is equal to the wood texture
    myShader.setInt("texture2", 1); // texture2 in the vertex shader is equal to the smiley texture

    // Render loop
    while(!glfwWindowShouldClose(window))
    {
#if IMGUI
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("ImGui window");
        ImGui::Text("Hello World");
        ImGui::End();
#endif
        processInput(window);

        glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, woodTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, smileyTexture);

        myShader.useProgram();

        glm::mat4 mat = glm::mat4(1.0f);
        mat = glm::translate(mat, glm::vec3(-0.25, -0.25, 0.0));
        mat = glm::rotate(mat, (float)glfwGetTime(), glm::vec3(0.0, 0.0, 1.0));

        glBindVertexArray(VAO1);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *)0);
        glUniformMatrix4fv(glGetUniformLocation(prog, "transform"), 1, GL_FALSE, glm::value_ptr(mat));

        mat = glm::mat4(1.0f);
        mat = glm::translate(mat, glm::vec3(-0.5, -0.5, 0.0));
        mat = glm::scale(mat, glm::vec3(sin(glfwGetTime())+1));

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *)0);
        glUniformMatrix4fv(glGetUniformLocation(prog, "transform"), 1, GL_FALSE, glm::value_ptr(mat));

#if IMGUI
        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

#if IMGUI
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
#endif

    // Optional
    glDeleteVertexArrays(1, &VAO1);
    glDeleteBuffers(1, &VBO1);
    glDeleteBuffers(1, &EBO1);

    // Not optional
    myShader.deleteProgram();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}