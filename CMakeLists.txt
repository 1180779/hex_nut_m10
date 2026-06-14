cmake_minimum_required(VERSION 4.2)
project(camcam_hex_nut_m10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCASCADE REQUIRED)

include(FetchContent)
FetchContent_Declare(
        svg
        GIT_REPOSITORY https://github.com/vincentlaucsb/svg.git
        GIT_TAG 0.5.0
)
FetchContent_MakeAvailable(svg)

add_executable(camcam_hex_nut_m10
        src/main.cpp
        src/HexNutParams.hpp
)

target_include_directories(camcam_hex_nut_m10 PRIVATE ${OpenCASCADE_INCLUDE_DIR})

target_link_libraries(camcam_hex_nut_m10 PRIVATE
        TKernel
        TKMath
        TKG2d
        TKG3d
        TKGeomBase
        TKBRep
        TKGeomAlgo
        TKTopAlgo
        TKPrim
        TKBO
        TKShHealing
        TKMesh
        TKSTEP
        TKSTL
        TKOffset
        svg
)
