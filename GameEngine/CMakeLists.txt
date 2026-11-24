# Dependencies
add_subdirectory(vendor/spdlog)
add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glm)

# GLAD
add_library(glad STATIC
    vendor/glad/src/glad.c
)

target_include_directories(glad
    PUBLIC
        vendor/glad/include
)

# GameEngine DLL
add_library(GameEngine SHARED)

if(MSVC)
    target_compile_options(GameEngine PRIVATE
        $<$<CONFIG:Debug>:/Zi>
    )

    target_link_options(GameEngine PRIVATE
        $<$<CONFIG:Debug>:/DEBUG>
    )
endif()

# Engine source files
file(GLOB_RECURSE GAMEENGINE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

target_sources(GameEngine PRIVATE
    ${GAMEENGINE_SOURCES}

    vendor/stb/stb_image.cpp

    # ImGui - compiled only once, inside GameEngine.dll
    vendor/imgui/imgui.cpp
    vendor/imgui/imgui_demo.cpp
    vendor/imgui/imgui_draw.cpp
    vendor/imgui/imgui_tables.cpp
    vendor/imgui/imgui_widgets.cpp
    vendor/imgui/backends/imgui_impl_glfw.cpp
    vendor/imgui/backends/imgui_impl_opengl3.cpp
)

target_precompile_headers(GameEngine PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/pch.h"
)

target_include_directories(GameEngine
    PUBLIC
        src
        vendor/stb
        vendor/imgui
)

target_compile_definitions(GameEngine
    PUBLIC
        GLM_ENABLE_EXPERIMENTAL

    PRIVATE
        GAMEENGINE_BUILD_DLL

        $<$<PLATFORM_ID:Windows>:GAMEENGINE_PLATFORM_WINDOWS>
        $<$<PLATFORM_ID:Linux>:GAMEENGINE_PLATFORM_LINUX>
        $<$<PLATFORM_ID:Darwin>:GAMEENGINE_PLATFORM_MACOS>

        $<$<CONFIG:Debug>:GAMEENGINE_DEBUG>
        $<$<CONFIG:Debug>:GAMEENGINE_ENABLE_ASSERTS>
        $<$<CONFIG:Release>:GAMEENGINE_RELEASE>

        # Export ImGui symbols from GameEngine.dll
        $<$<PLATFORM_ID:Windows>:IMGUI_API=__declspec\(dllexport\)>
)

target_link_libraries(GameEngine
    PUBLIC
        spdlog::spdlog
        glfw
        glad
        glm
        opengl32
)

add_custom_command(TARGET GameEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:GameEngine>"
        "$<TARGET_FILE_DIR:Sandbox>"
)