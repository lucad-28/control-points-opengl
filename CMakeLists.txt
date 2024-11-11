cmake_minimum_required(VERSION 3.10)
project(mi_proyecto)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS YES)


include_directories("C:/MinGW64/include")
link_directories("C:/MinGW64/lib")

add_executable(mi_proyecto main.cpp)
target_link_libraries(mi_proyecto freeglut opengl32 glu32)
