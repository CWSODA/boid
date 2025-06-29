cmake_minimum_required(VERSION 3.10)

project(Boids)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include ImGui as subdir
add_subdirectory(imgui)

# Add glad as lib
add_library(glad src/glad.c)

# Find GLFW
find_package(glfw3 REQUIRED)

# app is the executable
add_executable(boids 
    src/main.cpp 
    src/bird.cpp
    src/win_manager.cpp
    src/lib.cpp
)

include_directories(boids PRIVATE include)

# Link libs
target_link_libraries(boids 
    PRIVATE
    glad 
    glfw 
    imgui
)

target_include_directories(imgui PRIVATE /opt/homebrew/include)