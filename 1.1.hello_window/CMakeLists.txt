cmake_minimum_required(VERSION 3.23)  # Выбираем минимальную необходимую версию cmake

project(hello_window)                # Присваиваем имя проекту

add_executable(                      # Создаём цель (исполняемый файл)
  hello_window                       # Имя файла
  hello_window.cpp                   # Список файлов исходного кода
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
