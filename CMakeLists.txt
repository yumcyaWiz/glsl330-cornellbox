cmake_minimum_required(VERSION 3.12)

project(glsl_cornellbox LANGUAGES C CXX)

add_executable(main src/main.cpp)

# OpenGL
find_package(OpenGL REQUIRED)
target_link_libraries(main OpenGL::GL)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(deps/glfw)
target_link_libraries(main glfw)