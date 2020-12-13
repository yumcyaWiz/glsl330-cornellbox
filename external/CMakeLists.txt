# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)

# glad
add_library(glad glad/src/glad.c)
target_include_directories(glad SYSTEM PUBLIC glad/include)

# glm
add_subdirectory(glm)

# GLSL-Shader-Includes
add_library(glsl-shader-includes INTERFACE)
target_include_directories(glsl-shader-includes SYSTEM INTERFACE GLSL-Shader-Includes)