# Build spdlog before creating GameEngine
add_subdirectory(vendor/spdlog)
add_subdirectory(vendor/glfw)

add_library(GameEngine SHARED)

file(GLOB_RECURSE GAMEENGINE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

target_sources(GameEngine 
    PRIVATE
        ${GAMEENGINE_SOURCES}
)

target_precompile_headers(GameEngine
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src/pch.h"
)

target_include_directories(GameEngine
    PUBLIC
        src
)

target_compile_definitions(GameEngine
    PRIVATE
        GAMEENGINE_PLATFORM_WINDOWS
        GAMEENGINE_BUILD_DLL

        $<$<CONFIG:Debug>:GAMEENGINE_DEBUG>
        $<$<CONFIG:Debug>:GAMEENGINE_ENABLE_ASSERTS>

        $<$<CONFIG:Release>:GAMEENGINE_RELEASE>
)

target_link_libraries(GameEngine
    PUBLIC
        spdlog::spdlog
        glfw
)