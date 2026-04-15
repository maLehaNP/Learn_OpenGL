cmake_minimum_required(VERSION 3.23)

project(Face_culling)

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
    ./objects/
    ./shaders/
  DESTINATION .
)
