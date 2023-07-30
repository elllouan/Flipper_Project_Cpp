#include <iostream>
#include <fstream>
#include <string>

#include "shader.hpp"
#include "camera.hpp"

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
glm::vec3 cameraTarget = glm::vec3(0.0, 0.0, 0.0);
Camera fps = Camera(cameraPos, cameraTarget);

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


    // Loading the data for the cubes
    unsigned int VBO1, VAO1;
    {
        glGenVertexArrays(1, &VAO1);
        glGenBuffers(1, &VBO1);

        glBindVertexArray(VAO1);
        glBindBuffer(GL_ARRAY_BUFFER, VBO1);
        glBufferData(GL_ARRAY_BUFFER, sizeof(rectangles), rectangles, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void *)(3*sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    // Loading the data for the ground
    unsigned int VBO2, VAO2, EBO2;
    {
        glGenVertexArrays(1, &VAO2);
        glGenBuffers(1, &VBO2);
        glGenBuffers(1, &EBO2);

        glBindVertexArray(VAO2);
        glBindBuffer(GL_ARRAY_BUFFER, VBO2);
        glBufferData(GL_ARRAY_BUFFER, sizeof(ground), ground, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void *)(3*sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    unsigned int woodTexture, smileyTexture;
    glGenTextures(1, &woodTexture);
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

    unsigned int grassTexture;
    glGenTextures(1, &grassTexture);
    glBindTexture(GL_TEXTURE_2D, grassTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    data = stbi_load("C:\\Users\\Elouan THEOT\\Documents\\Programming\\c++\\Flipper_Project_Cpp\\img\\grass.jpg",
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

    // Create shaders programs
    Shader cubeShader = Shader();
    unsigned int progCube = cubeShader.CreateShaderProgram("vertexShaderCubes.vs", "fragmentShaderCubes.fs");

    cubeShader.UseProgram();
    cubeShader.SetInt("woodSampler", 0); // woodSampler in the vertex shader is equal to the wood texture
    cubeShader.SetInt("smileySampler", 1); // smileySampler in the vertex shader is equal to the smiley texture

    fps.CreateView();
    fps.Project(800.0f, 600.0f, near, far, fov);
    cubeShader.SetMatrix4fv("perspective", glm::value_ptr(fps.GetPerspectiveMat()));

    // Shader groundShader = Shader();
    // unsigned int progGround = groundShader.createShaderProgram("vertexShaderGround.vs", "fragmentShaderGround.fs");
    // groundShader.useProgram();
    // groundShader.setInt("groundSampler", 2);
    // glUniformMatrix4fv(glGetUniformLocation(progGround, "perspective"), 1, GL_FALSE, glm::value_ptr(projection));

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

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, woodTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, smileyTexture);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, grassTexture);

        fps.UpdateView();

        cubeShader.UseProgram();
        cubeShader.SetMatrix4fv("perspective", glm::value_ptr(fps.GetPerspectiveMat()));
        cubeShader.SetMatrix4fv("view", glm::value_ptr(fps.GetViewMat()));
        
        // Draw 10 cubes
        glBindVertexArray(VAO1);
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

        // groundShader.useProgram();
        // groundShader.setInt("groundSampler", 2);
        // glBindVertexArray(VAO2);
        // float shift = 0.2;
        // // Pass to next row
        // for (size_t i = 0; i < 15; i++)
        // {
        //     // One row of grass
        //     for (size_t j = 0; j < 10; j++)
        //     {
        //         glm::mat4 model = glm::mat4(1.0f);
        //         model = glm::scale(model, glm::vec3(1.5, 1.5, 10));
        //         model = glm::translate(model, glm::vec3(-0.9+shift*j, -0.9+shift*i,  1.0-i));
        //         model = glm::rotate(model, glm::radians(-75.0f) ,glm::vec3(1.0, 0.0, 0.0));
        //         glUniformMatrix4fv(glGetUniformLocation(progGround, "model"), 1, GL_FALSE, glm::value_ptr(model));
        //         glUniformMatrix4fv(glGetUniformLocation(progGround, "view"), 1, GL_FALSE, glm::value_ptr(view));
        //         glDrawArrays(GL_TRIANGLES, 0, 6);
        //     }
        // }
        

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
    glDeleteVertexArrays(1, &VAO2);
    glDeleteBuffers(1, &VBO2);
    glDeleteBuffers(1, &EBO2);

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
        fps.MoveForward(deltaTime, Mode::FPS);
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        fps.MoveBackwards(deltaTime, Mode::FPS);
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // Actually Q
        fps.MoveLeft(deltaTime);
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        fps.MoveRight(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
        fps.ZoomView(45.0f);
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

    fps.SpinView(yaw, pitch);
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
    fps.ZoomView(fov);
}

