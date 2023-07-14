#include <iostream>

// Either GLAD or GLEW
// #include <GL/glew.h>
#include <glad/glad.h>

#include <GLFW/glfw3.h> // Will drag system OpenGL headers

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

int main()
{
    glfwInit();
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

    // if (glewInit() != GLEW_OK)
    // {
    //     std::cout << "Failed to initialize GLEW" << std::endl;
    //     return -1;
    // }

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Print out the current version
    std::cout << glGetString(GL_VERSION) << std::endl;

    glViewport(0,0,1000,500);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    float vertices[6] = 
    {
        -0.5f,-0.5f,
         0.0f, 0.5f,
         0.5f,-0.5f
    };

    unsigned int triangleBuffer;
    glGenBuffers(1, &triangleBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, triangleBuffer);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), vertices, GL_STATIC_DRAW);

    // Render loop
    while(!glfwWindowShouldClose(window))
    {
        processInput(window);

        // Rendering commands
        glClear(GL_COLOR_BUFFER_BIT |   // This buffer contains the color info of each pixel within the frame
                GL_DEPTH_BUFFER_BIT );  // This buffer contains the depth info of each pixel within the frame

        // Does not work ...
        // glBegin(GL_TRIANGLES);
        // glVertex2f(-0.5f,-0.5f);
        // glVertex2f(0.0f,0.5f);
        // glVertex2f(0.5f,-0.5f);
        // glEnd();

        // Callback functions will be called if an event is detected
        glfwSwapBuffers(window);
        glfwPollEvents();    
    }

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