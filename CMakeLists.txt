cmake_minimum_required(VERSION 3.10)

project(KCraft C)

add_executable(
    KCraft
    src/main.c
    src/mvmath.c
    src/util.c
    src/chunk.c
    src/block.c
    src/game.c
    src/mesh.c
    src/world.c
    src/input.c
    src/entity.c
    src/bounding_box.c
    deps/glad/src/glad.c
    deps/lodepng/lodepng.c
)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build the GLFW test programs" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "Build example apps" FORCE)

add_subdirectory(deps/glfw)
include_directories(deps/glad/include)
include_directories(deps/glfw/include)
include_directories(deps/lodepng)

if(WIN32)
    target_link_libraries(KCraft wsock32 ws2_32 opengl32)
elseif(APPLE)
    target_link_libraries(KCraft
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
        "-framework OpenGL"
    )
elseif(UNIX)
    target_link_libraries(KCraft GL)
endif()

target_link_libraries(KCraft glfw ${GLFW_LIBRARIES})

add_custom_command(TARGET KCraft POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:KCraft>/res)