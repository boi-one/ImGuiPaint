cmake_minimum_required(VERSION 3.19)

set(CMAKE_TOOLCHAIN_FILE "C:/local/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file" FORCE)

include(${CMAKE_TOOLCHAIN_FILE})

set(CMAKE_CXX_STANDARD 20)

set(VCPKG_DEFAULT_TRIPLET "x64-windows")

project(ImGuiProject)

find_package(OpenGL REQUIRED)

find_package(SDL2 CONFIG REQUIRED)

find_package(ImGui REQUIRED)

find_package(GLEW REQUIRED)

add_executable(ImGuiApp main.cpp)

target_link_libraries(ImGuiApp PRIVATE SDL2::SDL2 GLEW::GLEW OpenGL::GL imgui::imgui)