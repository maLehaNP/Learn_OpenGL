cmake_minimum_required(VERSION 3.23)

project(Blending)

add_executable(${PROJECT_NAME}
  main.cpp
  glad.c
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
    shaderSingleColor.frag
    shaderSingleColor.vert
    backpack/
    ./bathroom_floor_tile_texture.jpg
    ./bathroom_floor_tile_texture_specular.jpg
    ./blending_transparent_window.png
    ./blending_transparent_window_specular.png
    ./container2.png
    ./container2_specular.png
  DESTINATION .
)
