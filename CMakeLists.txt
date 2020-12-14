cmake_minimum_required(VERSION 3.12)

project(glsl_cornellbox LANGUAGES C CXX)

# main
add_executable(main src/main.cpp)
target_compile_features(main PUBLIC cxx_std_17)
set_target_properties(main PROPERTIES CXX_EXTENSIONS OFF)
target_compile_options(main PRIVATE -Wall -Wextra)

# deps
add_subdirectory(external)

# OpenGL
find_package(OpenGL REQUIRED)
target_link_libraries(main OpenGL::GL)

# glad
target_link_libraries(main glad)

# GLFW
target_link_libraries(main glfw)

# glm
target_link_libraries(main glm)

# GLSL-Shader-Includes
target_link_libraries(main glsl-shader-includes)

# imgui
target_link_libraries(main imgui)

# copy shaders to build
add_custom_command(TARGET main POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/shaders $<TARGET_FILE_DIR:main>/shaders COMMENT "copying shaders" VERBATIM)