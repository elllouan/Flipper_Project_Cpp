#include <iostream>
#include <fstream>
#include <string>

#if IMGUI
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#endif

// Either GLAD or GLEW
#if WINDOW_MSVC
#include <glad/glad.h>
#else
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h> // Will drag system OpenGL headers

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

    // Shader Compilation Error Handling
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
 * @brief Create a complete VBO and VAO, ready to be rendered out.
 * This will call all necessary functions to generate, bind, attribute and populate both VBO and VAO.
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
    const char* glsl_version = "#version 130";
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

#if WINDOW_MSVC
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

    /* OpenGL uses buffers to render shapes out. Below is an example of a triangle. However, none of these functions:
     * glGenBuffers(), glBindBuffers(), glBufferData() specify what we aim to render and how we are going to do it.
     * Right now, those are just here to load data inside a buffer that we can use to render a triangle.
     * The data here is directly loaded on the GPU that waits our instruction (shader) before using this data.
     * A shader reads the data stored on the GPU and display it on the screen.
    */
    float positions1[6] = 
    {
        -0.5f,-0.5f,
         0.0f, 0.5f,
         0.5f,-0.5f
    };

    float positions2[6] = 
    {
        -0.8f,-0.8f,
         0.0f, 0.5f,
        -0.7f, 0.3f
    };

    // VBO stands for Vertex Buffer Object
    // VAO stands for Vertex Array Object

    /*
     * To render out several shapes such as 2 triangles, we have to consider that OpenGL works as a state machine.
     * It means that every vertex attribute specification made for a entity must be done before binding a new buffer.
     * Therefore, before binding to VAO2 and VBO2, I first need to specify everything for VBO1 and VBO2.
    */

    unsigned int VBO1, VBO2, VAO1, VAO2;
    glGenVertexArrays(1, &VAO1);
    glGenBuffers(1, &VBO1);
    CreateVertexAttribObject(VBO1, VAO1, GL_ARRAY_BUFFER, positions1, 6, GL_STATIC_DRAW, 0, 2, GL_FLOAT, 0);
    
    glGenVertexArrays(1, &VAO2);
    glGenBuffers(1, &VBO2);
    CreateVertexAttribObject(VBO2, VAO2, GL_ARRAY_BUFFER, positions2, 6, GL_STATIC_DRAW, 0, 2, GL_FLOAT, 0);

    // Generates an empty triangle, modifies the type of rasterization
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // Modifies the size of points, works only if GL_POINT is chosen
    // glPointSize(10.0f);

    // Draw a rectangle with 2 triangles using Element Buffer Object
    float rectangle[] = {
         0.3f,  0.3f,  // top right
         0.3f, -0.3f,  // bottom right
        -0.3f, -0.3f,  // bottom left
        -0.3f,  0.3f   // top left 
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };

    unsigned int VBO3, VAO3, EBO;
    glGenVertexArrays(1, &VAO3);
    glGenBuffers(1, &VBO3);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO3);
    // 2. copy our vertices array in a vertex buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBO3);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle), rectangle, GL_STATIC_DRAW);
    // 3. copy our index array in a element buffer for OpenGL to use
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // 4. then set the vertex attributes pointers
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // shader for printing triangles
    std::string vertexShader = parseShaderFile("vertexShaderTri.txt");
    std::string fragmentShader = parseShaderFile("fragmentShaderTri.txt");
    unsigned int shaderTri = CreateShader(vertexShader, fragmentShader);

    // shader for printing a rectangle
    fragmentShader = parseShaderFile("fragmentShaderRec.txt");
    unsigned int shaderRec = CreateShader(vertexShader, fragmentShader);

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

        glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw a red triangle
        glUseProgram(shaderTri);
        glBindVertexArray(VAO1);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Draw a yellow rectangle
        glUseProgram(shaderRec);
        glBindVertexArray(VAO3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Draw another red triangle
        glUseProgram(shaderTri);
        glBindVertexArray(VAO2);
        glDrawArrays(GL_TRIANGLES, 0, 3);

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

    
    // glDeleteVertexArrays(1, &VAO);
    // glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderTri);
    glDeleteProgram(shaderRec);
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