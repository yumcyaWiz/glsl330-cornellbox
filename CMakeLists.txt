cmake_minimum_required(VERSION 3.12)

project(glsl_cornellbox LANGUAGES C CXX)

add_executable(main src/main.cpp)

# deps
add_subdirectory(deps)

# OpenGL
find_package(OpenGL REQUIRED)
target_link_libraries(main OpenGL::GL)

# glad
target_link_libraries(main glad)

# GLFW
target_link_libraries(main glfw)