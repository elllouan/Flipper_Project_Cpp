#include <iostream>
#include <fstream>
#include <string>

#include "shader.hpp"
#include "camera.hpp"
#include "itemBuffer.hpp"
#include "entity.hpp"
#include "packet.hpp"

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
glm::vec3 x = glm::vec3(1.0, 0.0, 0.0);
glm::vec3 y = glm::vec3(0.0, 1.0, 0.0);
glm::vec3 z = glm::vec3(0.0, 0.0, 1.0);
glm::vec3 cameraPos = glm::vec3(0.0, 0.0, 3.0);
glm::vec3 cameraFront = glm::vec3(0.0, 0.0, -1.0);
glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);
glm::vec3 cameraTarget = glm::vec3(0.0, 0.0, 2.0);
// cam is declared global because it needs to be accessed from the callbacks
Camera cam = Camera(cameraPos, cameraTarget);

float deltaTime = 0.0f;
float lastTime = 0.0f;

unsigned int width = 800;
unsigned int height = 500;
float near = 0.1f;
float far = 100.0f;
float fov = 45.0f;

double x_mouse, y_mouse;
bool left_click_mouse, right_click_mouse;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

static double ClipMouseCursor(double pos_cursor, double max_pos)
{
    double new_pos = 2*(pos_cursor/max_pos) - 1;
    if (new_pos < -1)
    {
        return -1.0;
    }
    else if (new_pos > 1)
    {
        return 1.0;
    }
    else
    {
        return new_pos;
    }
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

    // cam is declared global because it needs to be accessed from the callbacks
    cam.CreateView();
    cam.CreatePerspective(800.0f, 600.0f, near, far, fov);

    // Create an item: position, texture, color and more
    unsigned int woodTexture, smileyTexture;
    ItemBuffer cubeBuffer = ItemBuffer(rectangles, sizeof(rectangles));
    // Adds position attribute
    cubeBuffer.AddVertexAttrib(0, 3, 5*sizeof(float), 0);
    // Adds texture attribute
    cubeBuffer.AddVertexAttrib(1, 2, 5*sizeof(float), 3*sizeof(float));
    // Sets textures
    cubeBuffer.AddTexture2D(woodTexture, "container.jpg");
    cubeBuffer.AddTexture2D(smileyTexture, "smiley.jpg");
    // Binds textures to GL_TEXTURE (16 max) in the same order they were added: e.g., container is bound to GL_TEXTURE0
    cubeBuffer.BindTextures();

    // Create shaders programs
    Shader shader = Shader();
    shader.CreateShaderProgram("vertexShaderCubes.vs", "fragmentShaderCubes.fs");

    // First, use the shader program
    shader.UseProgram();

    // Creates an packet that contains a camera, a shader and multiple entities (cubes here)
    Packet packet = Packet(&cam, &shader);

    // Adds all entities here
    for (size_t i = 0; i < 12; i++)
    {
        Entity cube = Entity(&cubeBuffer, 1.0f, positions[i], positions[i], deltaTime, glm::vec3(0.6));
        packet.AddEntity(std::move(cube));
    }

    packet.Render(deltaTime);

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

        for (size_t i = 0; i < 12; i++)
        {
            packet.UpdateEntity(glm::vec3(0.0f), glm::vec3(0.0f), deltaTime, glm::vec3(1.0f), i+1);
        }
        packet.CheckContact(deltaTime, x_mouse, y_mouse, left_click_mouse);
        
        packet.Render(deltaTime);

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
    
    // Not optional
    shader.DeleteProgram();
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
    x_mouse = ClipMouseCursor(xpos, static_cast<double>(width));
    y_mouse = ClipMouseCursor(ypos, static_cast<double>(height));
    std::cout << "DEBUG: x_mouse = " << x_mouse <<".\n";

    cam.SpinView(yaw, pitch);
    // (x_mouse - xentity)²+(y_mouse - yentity)² < bounding_radius
    // &&
    // zentity-bounding_radius < target < zentity+bounding_radius

    // ==> moveEntity(translate(direction))

}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        right_click_mouse = right_click_mouse ? false : true;
        std::cout << "DEBUG: Right click = " << right_click_mouse << ".\n";
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        left_click_mouse = left_click_mouse ? false : true;
        std::cout << "DEBUG: Left click = " << left_click_mouse << ".\n";
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= yoffset;
    cam.ZoomView(fov);
}

