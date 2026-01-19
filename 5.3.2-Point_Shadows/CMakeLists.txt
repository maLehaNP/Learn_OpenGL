cmake_minimum_required(VERSION 4.2)

project(Point_Shadows)

add_executable(${PROJECT_NAME}
    main.cpp
    glad.c
)

add_subdirectory(imgui)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ../includes
        imgui
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        libglfw3.a
        libassimp.so
        ImGui
)
