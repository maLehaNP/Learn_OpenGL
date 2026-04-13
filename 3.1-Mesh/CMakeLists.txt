cmake_minimum_required(VERSION 3.23)

project(Mesh)

add_executable(${PROJECT_NAME}
  main.cpp
  glad.c

  mesh.h
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
