cmake_minimum_required(VERSION 3.10)
project(vica VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directories early for better organization
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

add_subdirectory(vendor)

file(GLOB_RECURSE SOURCES "src/*.cpp" "app/*.cpp")

add_custom_command(
    OUTPUT "${CMAKE_BINARY_DIR}/bin/res"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/res" "${CMAKE_BINARY_DIR}/bin/res"
    DEPENDS "${CMAKE_SOURCE_DIR}/res"
    COMMENT "Copying resources"
    VERBATIM
)

add_custom_target(copy_resources ALL DEPENDS "${CMAKE_BINARY_DIR}/bin/res")

add_executable(vica ${SOURCES})

add_dependencies(vica copy_resources)

target_include_directories(vica
    PRIVATE src
    PRIVATE app
    PRIVATE vendor/stb_image
    PRIVATE vendor/imgui
    PRIVATE vendor/glad/include
)

target_link_libraries(vica
    PRIVATE glfw
    PRIVATE imgui
    PRIVATE glad
)