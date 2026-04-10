cmake_minimum_required(VERSION 3.23)  # Выбираем минимальную необходимую версию cmake

project(Triangles)                # Присваиваем имя проекту

add_executable(${PROJECT_NAME}
  2_triangles.cpp
  glad.c
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
    ../includes/
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    libglfw3.a
)
