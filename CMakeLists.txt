cmake_minimum_required(VERSION 3.10)

project(b C)

add_executable(
    b
    src/mvmath.c
    src/util.c
    src/bounding_box.c
    src/mesh.c
    src/block_data.c
    src/chunk.c
    src/entity.c
    src/world.c
    src/gui.c
    src/game.c
    src/input.c
    src/main.c
    deps/glad/src/glad.c
    deps/miniz/miniz.c
    deps/lodepng/lodepng.c)

add_executable(
    bserver
    src/server/server.c
    src/server/main.c
    deps/miniz/miniz.c)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build the GLFW test programs" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "Build example apps" FORCE)

add_subdirectory(deps/glfw)
include_directories(deps/miniz)
include_directories(deps/glad/include)
include_directories(deps/glfw/include)
include_directories(deps/lodepng)

if(WIN32)
    target_link_libraries(b wsock32 ws2_32 opengl32)
    target_link_libraries(bserver wsock32 ws2_32)
elseif(APPLE)
    target_link_libraries(b
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
        "-framework OpenGL"
    )
elseif(UNIX)
    target_link_libraries(b GL)
    target_link_libraries(bserver m)
endif()

target_link_libraries(b glfw ${GLFW_LIBRARIES})

add_custom_command(TARGET b POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:b>/res)