cmake_minimum_required(VERSION 4.2)

project(Point_Shadows)

add_executable(${PROJECT_NAME}
    main.cpp
    glad.c
)

add_subdirectory(imgui)

add_library(assimp SHARED IMPORTED)
set_property(TARGET assimp PROPERTY IMPORTED_LOCATION /usr/local/lib/libassimp.so)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ../includes
        ${CMAKE_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        libglfw3.a
        assimp
        imgui
)

file(COPY objects shaders DESTINATION .)