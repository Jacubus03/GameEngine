# Build spdlog before creating GameEngine
add_subdirectory(vendor/spdlog)

add_library(GameEngine SHARED)

file(GLOB_RECURSE GAMEENGINE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

target_sources(GameEngine PRIVATE
    ${GAMEENGINE_SOURCES}
)

target_include_directories(GameEngine
    PUBLIC
        src
)

target_compile_definitions(GameEngine
    PRIVATE
        GAMEENGINE_PLATFORM_WINDOWS
        GAMEENGINE_BUILD_DLL
)

target_compile_definitions(GameEngine PRIVATE
    $<$<CONFIG:Debug>:GAMEENGINE_DEBUG>
    $<$<CONFIG:Release>:GAMEENGINE_RELEASE>
)

target_link_libraries(GameEngine
    PUBLIC
        spdlog::spdlog
)