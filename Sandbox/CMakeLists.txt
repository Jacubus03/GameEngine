add_executable(Sandbox)

file(GLOB_RECURSE SANDBOX_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

target_sources(Sandbox 
    PRIVATE
        ${SANDBOX_SOURCES}
)

target_compile_definitions(Sandbox
    PRIVATE
        GAMEENGINE_PLATFORM_WINDOWS
)

target_compile_definitions(GameEngine 
    PRIVATE
        $<$<CONFIG:Debug>:GAMEENGINE_DEBUG>
        $<$<CONFIG:Release>:GAMEENGINE_RELEASE>
)

target_link_libraries(Sandbox
    PRIVATE
        GameEngine
)