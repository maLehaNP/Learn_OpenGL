cmake_minimum_required(VERSION 3.23)  # Выбираем минимальную необходимую версию cmake

project(Colors)  # Присваиваем имя проекту

add_executable(${PROJECT_NAME}
  colors.cpp
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
    shader.fs
    shader.vs
    light_cube.fs
    light_cube.vs
  DESTINATION .
)
