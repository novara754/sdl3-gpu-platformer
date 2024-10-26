cmake_minimum_required(VERSION 3.27)
project(sponza_scene)

add_subdirectory(vendor/sdl3)

find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3)

set(CMAKE_CXX_STANDARD 23)

add_executable(platformer
        src/main.cpp
)

target_compile_definitions(platformer PRIVATE
        _CRT_SECURE_NO_WARNINGS
        GLFW_INCLUDE_NONE
        GLM_FORCE_EXPLICIT_CTOR
)

target_include_directories(platformer PRIVATE ${Stb_INCLUDE_DIR})
target_link_libraries(platformer PRIVATE fmt::fmt)
target_link_libraries(platformer PRIVATE spdlog::spdlog)
target_link_libraries(platformer PRIVATE glm::glm)
target_link_libraries(platformer PRIVATE imgui::imgui)
target_link_libraries(platformer PRIVATE SDL3::SDL3)
