cmake_minimum_required(VERSION 3.16)

project(PROYECTO)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenMP REQUIRED)

include_directories(include)

add_library(proyecto
    src/SUDOKU.cpp
    src/utils.cpp
    src/PSO.cpp
    src/MEMETICO.cpp
    src/RECOCIDO.cpp
    src/TABU.cpp
)

# Crear ejecutable
add_executable(main_exec main/main_SUDOKU.cpp)

target_link_libraries(main_exec proyecto)

target_link_libraries(main_exec
    proyecto
    OpenMP::OpenMP_CXX
)