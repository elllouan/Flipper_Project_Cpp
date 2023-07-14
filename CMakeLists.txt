cmake_minimum_required(VERSION 3.22.1)
project(flipper VERSION 0.1.0 LANGUAGES C CXX)

set(EXE flipper)

set(IMGUI_ROOT_FOLDER ${CMAKE_SOURCE_DIR}/dependencies/imgui)
set(GLFW_ROOT_FOLDER ${CMAKE_SOURCE_DIR}/dependencies/glfw)
set(GLEW_ROOT_FOLDER ${CMAKE_SOURCE_DIR}/dependencies/glew)

include(CTest)
enable_testing()

file(GLOB IMGUI_SRC_FILE ${IMGUI_ROOT_FOLDER}/*.cpp)

if(MSVC)
message("Compiling with Microsoft Visual Compiler (MSVC)")

add_executable(${EXE} 
            src/main.cpp
            ${IMGUI_ROOT_FOLDER}/backends/imgui_impl_opengl3.cpp
            ${IMGUI_ROOT_FOLDER}/backends/imgui_impl_glfw.cpp
            ${IMGUI_SRC_FILE}
            ${IMGUI_ROOT_FOLDER}/misc/cpp/imgui_stdlib.cpp
            src/glad.c
            )

target_include_directories(${EXE}
                PUBLIC ${IMGUI_ROOT_FOLDER}
                PUBLIC ${IMGUI_ROOT_FOLDER}/misc/cpp
                PUBLIC ${IMGUI_ROOT_FOLDER}/backends
                PUBLIC ${SFML_ROOT_FOLDER}/include
                # PUBLIC "C:\\glfw-3.3.8.bin.WIN32\\include"  # path to GLFW headers on Windows system
                PUBLIC ${GLFW_ROOT_FOLDER}/include
                # PUBLIC ${GLEW_ROOT_FOLDER}/include
                PRIVATE ${CMAKE_SOURCE_DIR}/include
                )

# target_link_directories(${EXE} PUBLIC "C:\\glfw-3.3.8.bin.WIN64\\lib-vc2022") # path to GLFW libs on Windows system
if(CMAKE_BUILD_TYPE MATCHES "Debug")
    target_link_directories(${EXE} PUBLIC ${GLFW_ROOT_FOLDER}/build/src/Debug)
else()
    target_link_directories(${EXE} PUBLIC ${GLFW_ROOT_FOLDER}/build/src/Release)
endif()

# target_link_directories(${EXE} PUBLIC ${GLEW_ROOT_FOLDER}/lib/Release/x64)
target_link_libraries(${EXE} PUBLIC glfw3)

else()
message("Compiling with GNU Compiler.")

add_executable(${EXE} 
            src/main.cpp
            ${IMGUI_ROOT_FOLDER}/backends/imgui_impl_opengl3.cpp
            ${IMGUI_ROOT_FOLDER}/backends/imgui_impl_glfw.cpp
            ${IMGUI_SRC_FILE}
            ${IMGUI_ROOT_FOLDER}/misc/cpp/imgui_stdlib.cpp
            src/glad.c
            )

target_include_directories(${EXE}
                PUBLIC ${IMGUI_ROOT_FOLDER}
                PUBLIC ${IMGUI_ROOT_FOLDER}/misc/cpp
                PUBLIC ${IMGUI_ROOT_FOLDER}/backends
                # PUBLIC ${GLEW_ROOT_FOLDER}/include
                PRIVATE ${CMAKE_SOURCE_DIR}/include
                )

find_package(X11 REQUIRED)

# target_link_directories(${EXE} PUBLIC ${GLEW_ROOT_FOLDER}/lib/Release/x64)
target_link_libraries(${EXE} PUBLIC glfw GL ${X11_LIBRARIES} pthread ${X11_Xrandr_LIB} dl)

endif()

# Working with OpenGL on both platforms
find_package(OpenGL REQUIRED)

if(OPENGL_FOUND)
message("OPENGL was found.")

    if(OpenGL_OpenGL_FOUND)
    message("The system has an OpenGL library: " ${OPENGL_gl_LIBRARY})
    target_link_libraries(${EXE} PUBLIC ${OPENGL_gl_LIBRARY})
    endif()

    if(OpenGL_GLU_FOUND)
    message("The system has a GLU library: " ${OPENGL_glu_LIBRARY})
    target_link_libraries(${EXE} PUBLIC ${OPENGL_glu_LIBRARY})
    endif()
    
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
