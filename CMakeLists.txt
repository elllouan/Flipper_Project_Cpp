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
add_definitions(-DWINDOW_MSVC) # add_compile_definiftions does not work for this version of cmake

add_executable(${EXE} 
            src/main.cpp
            # src/main_test.cpp
            src/glad.c
            # ${IMGUI_ROOT_FOLDER}/backends/imgui_impl_opengl3.cpp
            # ${IMGUI_ROOT_FOLDER}/backends/imgui_impl_glfw.cpp
            # ${IMGUI_SRC_FILE}
            # ${IMGUI_ROOT_FOLDER}/misc/cpp/imgui_stdlib.cpp
            )

target_include_directories(${EXE}
                PRIVATE ${CMAKE_SOURCE_DIR}/include
                PUBLIC ${GLEW_ROOT_FOLDER}/include
                PUBLIC ${GLFW_ROOT_FOLDER}/include
                # PUBLIC ${IMGUI_ROOT_FOLDER}
                # PUBLIC ${IMGUI_ROOT_FOLDER}/misc/cpp
                # PUBLIC ${IMGUI_ROOT_FOLDER}/backends
                # PUBLIC "C:\\glfw-3.3.8.bin.WIN32\\include"  # path to GLFW headers on Windows system
                )

# target_link_directories(${EXE} PUBLIC "C:\\glfw-3.3.8.bin.WIN64\\lib-vc2022") # path to GLFW binary libs on Windows system
if(CMAKE_BUILD_TYPE MATCHES "Debug")
    message("Compiling in Debug mode")
    target_link_directories(${EXE} PUBLIC ${GLFW_ROOT_FOLDER}/build/src/Debug)
    target_link_libraries(${EXE} PUBLIC glfw3)
else()
    message("Compiling in Release mode")
    target_link_directories(${EXE} PUBLIC ${GLFW_ROOT_FOLDER}/build/src/Release)
    target_link_libraries(${EXE} PUBLIC glfw3)
endif()

else()
message("Compiling with GNU Compiler.")

add_executable(${EXE} 
            src/main.cpp
            # ${IMGUI_ROOT_FOLDER}/backends/imgui_impl_opengl3.cpp
            # ${IMGUI_ROOT_FOLDER}/backends/imgui_impl_glfw.cpp
            # ${IMGUI_SRC_FILE}
            # ${IMGUI_ROOT_FOLDER}/misc/cpp/imgui_stdlib.cpp
            )

# target_include_directories(${EXE}
#                 PUBLIC ${IMGUI_ROOT_FOLDER}
#                 PUBLIC ${IMGUI_ROOT_FOLDER}/misc/cpp
#                 PUBLIC ${IMGUI_ROOT_FOLDER}/backends
#                 # PUBLIC ${GLEW_ROOT_FOLDER}/include
#                 )

find_package(GLEW QUIET)
if(GLEW_FOUND)
message("GLEW found, library path: " ${GLEW_LIBRARIES})
message("GLEW found, includes path: " ${GLEW_INCLUDE_DIRS})
endif()

find_package(X11 REQUIRED)
if(X11_FOUND)
message("X11 found, libraries path: " ${X11_LIBRARIES})
message("X11 found, includes path: " ${X11_INCLUDE_DIR})
endif()

target_link_libraries(${EXE} PUBLIC glfw GL ICE X11 Xext GLEW)

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

option(IMGUI "Enable ImGui code." OFF)
if(${IMGUI})
add_compile_definitions(-DIMGUI)
endif()



set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
