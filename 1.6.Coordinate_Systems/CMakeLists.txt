cmake_minimum_required(VERSION 3.23)  # Выбираем минимальную необходимую версию cmake

project(Coordinate_systems)  # Присваиваем имя проекту

add_executable(${PROJECT_NAME}
  Coordinate_Systems.cpp
  glad.c

  shader_m.h
  stb_image.h
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
    awesomeface.png
    container.jpg
    wall.jpg
  DESTINATION .
)
