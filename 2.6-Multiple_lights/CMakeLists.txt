cmake_minimum_required(VERSION 3.23)

project(Multiple_lights)

add_executable(${PROJECT_NAME}
  multiple_lights.cpp
  glad.c

  shader_m.h
  stb_image.h
  Camera.h
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
    ../includes/
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    libglfw3.a
)

file(
  COPY
    light_cube.frag
    light_cube.vert
    shader.frag
    shader.vert
    container2.png
    container2_specular.png
  DESTINATION .
)
