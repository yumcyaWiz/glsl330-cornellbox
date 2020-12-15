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

# imgui
add_library(imgui imgui/imgui.cpp imgui/imgui_widgets.cpp imgui/imgui_draw.cpp imgui/imgui_demo.cpp)
target_include_directories(imgui SYSTEM PUBLIC imgui)

# imgui impl
add_library(imgui_impl imgui/examples/imgui_impl_glfw.cpp imgui/examples/imgui_impl_opengl3.cpp)
target_compile_definitions(imgui_impl PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLAD)
target_link_libraries(imgui_impl imgui)
target_link_libraries(imgui_impl glfw)
target_link_libraries(imgui_impl glad)
target_include_directories(imgui_impl SYSTEM PUBLIC imgui/examples)