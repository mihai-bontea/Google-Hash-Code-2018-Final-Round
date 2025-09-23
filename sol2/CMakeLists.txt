cmake_minimum_required(VERSION 3.27)
project(sol2)

set(CMAKE_CXX_STANDARD 23)

add_executable(sol2 main.cpp
        Data.h
        Definitions.h
        BuildingProject.h
        CollisionMap.h)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fopenmp")