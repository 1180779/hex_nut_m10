cmake_minimum_required(VERSION 4.2)
project(camcam_hex_nut_m10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCASCADE REQUIRED)

add_executable(camcam_hex_nut_m10
        src/main.cpp
        src/HexNutParams.hpp
        #        src/Params.cpp
#        src/NutBuilder.cpp
#        src/Exporters.cpp
#        src/SvgPlotter.cpp
)

target_include_directories(camcam_hex_nut_m10 PRIVATE ${OpenCASCADE_INCLUDE_DIR})

target_link_libraries(camcam_hex_nut_m10 PRIVATE
        TKernel TKMath
        TKG2d TKG3d TKGeomBase
        TKBRep TKGeomAlgo TKTopAlgo TKPrim TKBO TKShHealing
        TKMesh
        TKSTEP TKSTL
)
