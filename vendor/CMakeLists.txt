cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++${CMAKE_CXX_STANDARD}")

project(imgui VERSION 0.1.0 LANGUAGES CXX)

file(GLOB SOURCES "imgui/backends/imgui_impl_opengl3.cpp" "imgui/*.cpp" "imgui/misc/cpp/imgui_stdlib.cpp")

add_library(imgui STATIC ${SOURCES})
target_include_directories(imgui PRIVATE imgui)
target_link_libraries(imgui PRIVATE glfw)

project(glad VERSION 1.0.0 LANGUAGES C)

file(GLOB SOURCES "glad/src/glad.c")

add_library(glad STATIC ${SOURCES})
target_include_directories(glad PRIVATE glad/include)