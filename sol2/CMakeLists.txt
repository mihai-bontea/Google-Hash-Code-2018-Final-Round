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

set(CMAKE_CXX_FLAGS_RELEASE
        "-O3 -Ofast -march=native -mtune=native \
     -ffast-math -funroll-loops -fomit-frame-pointer \
     -finline-functions -finline-functions-called-once \
     -fno-exceptions -fno-rtti -DNDEBUG"
)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fdata-sections -ffunction-sections")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -Wl,--gc-sections")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -ffp-contract=fast -freciprocal-math -fassociative-math -fvect-cost-model=dynamic")