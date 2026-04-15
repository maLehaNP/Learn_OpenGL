cmake_minimum_required(VERSION 3.23)

project(Model)

add_executable(${PROJECT_NAME}
  main.cpp
  glad.c

  #mesh.h
  #shader_m.h
  #stb_image.h
  #Camera.h
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
    ../includes/
)

add_library(assimp SHARED IMPORTED)
set_property(TARGET assimp PROPERTY IMPORTED_LOCATION /usr/local/lib/libassimp.so)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    libglfw3.a
    assimp
)

file(
  COPY
    light_cube.frag
    light_cube.vert
    shader.frag
    shader.vert
    simple_shader.frag
    simple_shader.vert
    container2.png
    container2_specular.png
  DESTINATION .
)
