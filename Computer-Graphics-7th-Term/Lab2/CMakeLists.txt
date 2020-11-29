cmake_minimum_required(VERSION 3.14)
project(cglab2)

set(CMAKE_CXX_STANDARD 17)
add_executable(cglab2 main.cpp glutils.cpp camera.cpp model.cpp mesh.cpp texloader.cpp)

find_package(OpenGL REQUIRED COMPONENTS OpenGL)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(assimp REQUIRED)

find_library(SPNG spng REQUIRED)

add_dependencies(cglab2 OpenGL::OpenGL)

target_link_libraries(cglab2 OpenGL::OpenGL GLEW::GLEW glfw glm assimp ${SPNG})
