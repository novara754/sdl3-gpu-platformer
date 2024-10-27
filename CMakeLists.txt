cmake_minimum_required(VERSION 3.25)
project(platformer)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)

set(SDL_STATIC ON)
set(SPDLOG_FMT_EXTERNAL ON CACHE BOOL "" FORCE)
set(SPDLOG_WCHAR_TO_UTF8_SUPPORT ON CACHE BOOL "" FORCE)

add_subdirectory(vendor/sdl3)

find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(imgui CONFIG REQUIRED)

find_program(glslc_executable NAMES glslc HINTS Vulkan::glslc)

add_executable(platformer
        src/main.cpp
)

target_compile_definitions(platformer PRIVATE
        _CRT_SECURE_NO_WARNINGS
        GLFW_INCLUDE_NONE
        GLM_FORCE_EXPLICIT_CTOR
)

target_compile_options(platformer PRIVATE -Wall -Werror -Wextra -Wpedantic)

target_include_directories(platformer PRIVATE ${Stb_INCLUDE_DIR})
target_link_libraries(platformer PRIVATE fmt::fmt)
target_link_libraries(platformer PRIVATE spdlog::spdlog)
target_link_libraries(platformer PRIVATE glm::glm)
target_link_libraries(platformer PRIVATE imgui::imgui)
target_link_libraries(platformer PRIVATE SDL3::SDL3-static)

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
        src/triangle.frag
        src/triangle.vert
)
