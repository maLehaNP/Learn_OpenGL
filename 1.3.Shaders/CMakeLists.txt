cmake_minimum_required(VERSION 3.23)  # Выбираем минимальную необходимую версию cmake

project(Shaders)  # Присваиваем имя проекту

add_executable(${PROJECT_NAME}
  Shaders.cpp
  glad.c

  shader_s.h
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
    ../includes/
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    libglfw3.a
)

file(COPY shader.fs shader.vs DESTINATION .)
