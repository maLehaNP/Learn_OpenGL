cmake_minimum_required(VERSION 4.2)

project(Advanced_GLSL)

add_executable(${PROJECT_NAME}
  glad.c
  main.cpp
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
    ../includes
)

add_library(assimp SHARED IMPORTED)
set_property(TARGET assimp PROPERTY IMPORTED_LOCATION /usr/local/lib/libassimp.so)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    libglfw3.a
    assimp
)

file(COPY objects shaders skybox DESTINATION .)
