cmake_minimum_required(VERSION 3.27)
project(sol1)

set(CMAKE_CXX_STANDARD 23)

add_executable(sol1
        BuildingProject.h
        CollisionMap.h
        Data.h
        main.cpp
        Solver.h
        SimulationState.h
        Definitions.h)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fopenmp")