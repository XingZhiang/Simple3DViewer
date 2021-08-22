cmake_minimum_required(VERSION 3.19)
set(CMAKE_TOOLCHAIN_FILE E:/Tool/vcpkg/scripts/buildsystems/vcpkg.cmake)
project(Viewer)

set(CMAKE_CXX_STANDARD 17)

find_package(glfw3 CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
add_executable(Viewer
        main.cpp glad.c Shader.h stb_image.h main.cpp Camera.h Mesh.h Model.h)

target_link_libraries(Viewer opengl32)
target_link_libraries(Viewer User32)
target_link_libraries(Viewer assimp::assimp)
target_link_libraries(Viewer glm::glm)
target_link_libraries(Viewer imgui::imgui)
