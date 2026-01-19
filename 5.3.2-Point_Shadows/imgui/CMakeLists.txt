add_library(ImGui)

target_sources(ImGui
    PRIVATE
        imgui.cpp
        imgui_impl_glfw.cpp
        imgui_impl_opengl3.cpp
    
    PUBLIC
        FILE_SET HEADERS
        FILES
            imgui.h
            imgui_impl_glfw.h
            imgui_impl_opengl3.h
            imconfig.h
            imgui_impl_opengl3_loader.h
            imgui_internal.h
)