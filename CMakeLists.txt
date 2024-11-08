cmake_minimum_required(VERSION 3.10)
project(mi_proyecto)

set(CMAKE_CXX_STANDARD 17)



add_executable(mi_proyecto main.cpp)

# Enlazar las bibliotecas necesarias
target_link_libraries(mi_proyecto glut opengl32 glu32)

