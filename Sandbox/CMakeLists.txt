# Sandbox executable
add_executable(Sandbox)

file(GLOB_RECURSE SANDBOX_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"

    # Scripts are compiled with the project and register themselves
    "${CMAKE_CURRENT_SOURCE_DIR}/Assets/Scripts/*.cpp"
)

target_sources(Sandbox PRIVATE
    ${SANDBOX_SOURCES}
)

target_include_directories(Sandbox
    PRIVATE
        src
)

target_compile_definitions(Sandbox
    PRIVATE
        GAMEENGINE_PLATFORM_WINDOWS
        SANDBOX_SOURCE_ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/Assets"
        SANDBOX_EXECUTABLE_NAME="$<TARGET_FILE_NAME:Sandbox>"
)

# Import ImGui symbols from GameEngine.dll
if(WIN32)
    target_compile_definitions(Sandbox
        PRIVATE
            "IMGUI_API=__declspec(dllimport)"
    )
endif()

target_link_libraries(Sandbox
    PRIVATE
        GameEngine
)

# Copy engine DLL and assets next to Sandbox.exe
add_custom_command(TARGET Sandbox POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:GameEngine>"
        "$<TARGET_FILE_DIR:Sandbox>"

    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/Assets"
        "$<TARGET_FILE_DIR:Sandbox>/Assets"

    COMMENT "Copying GameEngine.dll and Sandbox assets"
)