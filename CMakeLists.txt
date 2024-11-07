cmake_minimum_required(VERSION 3.11)
project(platformer)

include(FetchContent)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)

set(SDL_STATIC ON)
set(SDL_SHARED OFF)

FetchContent_Declare(
        spdlog
        SYSTEM
        GIT_REPOSITORY "https://github.com/gabime/spdlog"
        GIT_TAG "v1.14.1"
)
FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(
        SDL3
        SYSTEM
        GIT_REPOSITORY "https://github.com/libsdl-org/SDL"
        GIT_TAG "68cc173d9293e890ba93feaed1d3dc17742aa9b3"
)
FetchContent_MakeAvailable(SDL3)

FetchContent_Declare(
        glm
        SYSTEM
        GIT_REPOSITORY "https://github.com/g-truc/glm"
        GIT_TAG "1.0.1"
)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(
        stb
        SYSTEM
        GIT_REPOSITORY "https://github.com/nothings/stb"
        GIT_TAG "2e2bef463a5b53ddf8bb788e25da6b8506314c08"
)
FetchContent_MakeAvailable(stb)

FetchContent_Declare(
        box2d
        SYSTEM
        GIT_REPOSITORY "https://github.com/erincatto/box2d"
        GIT_TAG "v3.0.0"
)
FetchContent_MakeAvailable(box2d)

FetchContent_Declare(
        entt
        SYSTEM
        GIT_REPOSITORY "https://github.com/skypjack/entt"
        GIT_TAG "v3.14.0"
)
FetchContent_MakeAvailable(entt)

find_program(glslc_executable NAMES glslc HINTS Vulkan::glslc)

add_executable(platformer
        src/main.cpp
        src/engine.cpp
        src/read_file.cpp
        src/sprite_render_pass.cpp
        src/texture.cpp
        src/game.cpp
        src/physics.cpp
        src/audio.cpp
        src/input.cpp
        src/renderer.cpp
        src/stb_impl.c
)

target_compile_definitions(platformer PRIVATE
        _CRT_SECURE_NO_WARNINGS
        GLM_FORCE_EXPLICIT_CTOR
        GLM_ENABLE_EXPERIMENTAL
)

target_compile_options(platformer PRIVATE -Wall -Werror -Wextra -Wpedantic)

target_include_directories(platformer PRIVATE ${stb_SOURCE_DIR})
target_link_libraries(platformer PRIVATE spdlog::spdlog)
target_link_libraries(platformer PRIVATE glm::glm)
target_link_libraries(platformer PRIVATE SDL3::SDL3-static)
target_link_libraries(platformer PRIVATE box2d)
target_link_libraries(platformer PRIVATE EnTT::EnTT)

function(compile_shader target)
        cmake_parse_arguments(PARSE_ARGV 1 arg "" "ENV;FORMAT" "SOURCES")

        foreach(source ${arg_SOURCES})
                add_custom_command(
                        OUTPUT ${source}.${arg_FORMAT}
                        DEPENDS ${source}

                        DEPFILE ${source}.d
                        COMMAND
                        ${glslc_executable}
                        $<$<BOOL:${arg_ENV}>:--target-env=${arg_ENV}>
                        $<$<BOOL:${arg_FORMAT}>:-mfmt=${arg_FORMAT}>
                        -MD -MF ${source}.d
                        -o ${source}.${arg_FORMAT}
                        ${CMAKE_CURRENT_SOURCE_DIR}/${source}
                )
                target_sources(${target} PRIVATE ${source}.${arg_FORMAT})
        endforeach()
endfunction()

compile_shader(platformer ENV vulkan1.1 FORMAT bin
        SOURCES
        shaders/sprite.frag
        shaders/sprite.vert
)

add_custom_command(
        TARGET platformer POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_SOURCE_DIR}/assets/" "${CMAKE_CURRENT_BINARY_DIR}/assets"
)
