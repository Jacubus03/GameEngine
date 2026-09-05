add_executable(Sandbox)

file(GLOB_RECURSE SANDBOX_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

target_sources(Sandbox 
    PRIVATE
        ${SANDBOX_SOURCES}
)

add_custom_command(TARGET Sandbox POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:GameEngine>
        $<TARGET_FILE_DIR:Sandbox>
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