#include <iostream>
#include <fstream>
#include <string>

#include "shader.hpp"
#include "camera.hpp"
#include "item.hpp"

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

// Some global variables
float yaw = 0.0, pitch = 0.0;
glm::vec3 cameraPos = glm::vec3(0.0, 0.0, 3.0);
glm::vec3 cameraFront = glm::vec3(0.0, 0.0, -1.0);
glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);
glm::vec3 cameraTarget = glm::vec3(0.0, 0.0, 2.0);
// cam is declared global because it needs to be accessed from the callbacks
Camera cam = Camera(cameraPos, cameraTarget);

float deltaTime = 0.0;
float lastTime = 0.0;

unsigned int width = 800;
unsigned int height = 500;
float near = 0.1f;
float far = 100.0f;
float fov = 45.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

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

int main()
{
    glfwInit();
    const char* glsl_version = "#version 330";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(width, height, "LearnOpenGL", NULL, NULL);
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

    // Window's dimensions
    glViewport(0,0,width,height);
    // Sets a callback for window resizing
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
    /*    positions    |  textures  */
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    glm::vec3 positions[] = {
        glm::vec3(0.0, 0.0, 0.0),
        glm::vec3(0.5, 0.5, -0.5),
        glm::vec3(-0.5, 0.7, 0.1),
        glm::vec3(0.0, -0.5, 0.5),
        glm::vec3(3.0, -2.0, -14.0),
        glm::vec3(0.0, 0.0, -4.0),
        glm::vec3(-5.0, 0.5, -0.5),
        glm::vec3(5.0, 6.7, -5.0),
        glm::vec3(-10.0, -17, -5.0),
        glm::vec3(15.0, 10.0, -10.0),
    };

    float ground[] = {
        -0.1, -0.1, 0.0, 0.0, 0.0,
        -0.1,  0.1, 0.0, 0.0, 1.0,
         0.1, -0.1, 0.0, 1.0, 0.0,

        -0.1,  0.1, 0.0, 0.0, 1.0,
         0.1,  0.1, 0.0, 1.0, 1.0,
         0.1, -0.1, 0.0, 1.0, 0.0,
    };

    glm::vec3 tiles[] = {
        glm::vec3(-0.9, -0.9,  1.0),
        glm::vec3(-0.9, -0.9,  0.0),
        glm::vec3(-0.9, -0.9, -1.0),
        glm::vec3(-0.9, -0.9, -2.0),
        glm::vec3(-0.9, -0.9, -3.0),
        glm::vec3(-0.9, -0.9, -15.0),
        glm::vec3(-0.9, -0.9, -18.0),
        glm::vec3(-0.9, -0.9, -22.0),
    };

    // Create an item
    unsigned int woodTexture, smileyTexture;
    Item cube = Item(rectangles, sizeof(rectangles));
    cube.EnableVertexAttrib(0, 3, 5*sizeof(float), 0);
    cube.EnableVertexAttrib(1, 2, 5*sizeof(float), 3*sizeof(float));
    cube.AddTexture2D(woodTexture, "container.jpg");
    cube.AddTexture2D(smileyTexture, "smiley.jpg");
    cube.BindTextures();

    // Create shaders programs
    Shader cubeShader = Shader();
    unsigned int progCube = cubeShader.CreateShaderProgram("vertexShaderCubes.vs", "fragmentShaderCubes.fs");

    cubeShader.UseProgram();
    cubeShader.SetInt("woodSampler", 0); // woodSampler in the vertex shader is equal to the wood texture
    cubeShader.SetInt("smileySampler", 1); // smileySampler in the vertex shader is equal to the smiley texture

    // cam is declared global because it needs to be accessed from the callbacks
    cam.CreateView();
    cam.Project(800.0f, 600.0f, near, far, fov);
    cubeShader.SetMatrix4fv("perspective", glm::value_ptr(cam.GetPerspectiveMat()));

    // Some settings
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Draws filled primitives
    glEnable(GL_DEPTH_TEST); // Enables depth consideration when drawing primitives

    // Mouse cursor settings
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Locks the cursor to the window
    glfwSetCursorPosCallback(window, cursor_position_callback); // Sets a callback for the cursor
    glfwSetMouseButtonCallback(window, mouse_button_callback); // Sets a callback for mouse's buttons
    glfwSetScrollCallback(window, scroll_callback); // Sets a callback for mouse's buttons

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
        deltaTime = glfwGetTime() - lastTime;
        processInput(window);
        lastTime = glfwGetTime();

        glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Generates a new lookAt/view matrix
        cam.UpdateView();

        cubeShader.UseProgram();
        cubeShader.SetMatrix4fv("perspective", glm::value_ptr(cam.GetPerspectiveMat()));
        cubeShader.SetMatrix4fv("view", glm::value_ptr(cam.GetViewMat()));
        
        // Draw 10 cubes
        cube.Bind();
        for (size_t i = 0; i < 12; i++)
        {
            // The model matrix locates the primitives in space, therefore each primitive has its own model matrix (i.e., location)
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, positions[i]);
            model = glm::rotate(model, (float)glfwGetTime(), positions[i]);
            model = glm::scale(model, glm::vec3(0.5));
            cubeShader.SetMatrix4fv("model", glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }        

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
    // glDeleteVertexArrays(1, &VAO1);
    // glDeleteBuffers(1, &VBO1);
    // glDeleteVertexArrays(1, &VAO2);
    // glDeleteBuffers(1, &VBO2);
    // glDeleteBuffers(1, &EBO2);

    // Not optional
    cubeShader.DeleteProgram();
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
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) // Actually Z
        cam.MoveForward(deltaTime);
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cam.MoveBackwards(deltaTime);
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // Actually Q
        cam.MoveLeft(deltaTime);
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cam.MoveRight(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
        cam.ZoomView(45.0f);
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    // prev_xpos and prev_ypos are the x and y cursor's last frame coordinates
    static double prev_xpos = xpos;
    static double prev_ypos = ypos;
    // R = glm::length(direction)
    double R = 1;
    // To dim the speed
    double sensitivity = 0.1;
    // Compute how much distance was traveled
    double deltaX = (xpos-prev_xpos)*sensitivity;
    double deltaY = (ypos-prev_ypos)*sensitivity;
    // Deduce the correct angle offset
    yaw -= glm::atan(deltaX/R);
    pitch += glm::atan(deltaY/R);
    // Update cursor last frame coordinates
    prev_xpos = xpos;
    prev_ypos = ypos;

    cam.SpinView(yaw, pitch);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        return;
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        return;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= yoffset;
    cam.ZoomView(fov);
}

