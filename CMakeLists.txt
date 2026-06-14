cmake_minimum_required(VERSION 4.2)
project(cadcam_hex_nut_m10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (WIN32)
    # Can be overridden via -DOpenCASCADE_ROOT=... or the OPENCASCADE_ROOT env var
    if (DEFINED ENV{OPENCASCADE_ROOT})
        set(OpenCASCADE_ROOT "$ENV{OPENCASCADE_ROOT}" CACHE PATH "OpenCASCADE install root")
    else()
        set(OpenCASCADE_ROOT
                "C:/Users/rdkgs/Downloads/occt-combined-with-debug-pch/opencascade-8.0.0-vc14-64-pch-with-debug-combined/opencascade-8.0.0-vc14-64"
                CACHE PATH "OpenCASCADE install root"
        )
    endif()

    # Can be overridden via -DOpenCASCADE_THIRDPARTY_ROOT=... or the OPENCASCADE_THIRDPARTY_ROOT env var
    if (DEFINED ENV{OPENCASCADE_THIRDPARTY_ROOT})
        set(OpenCASCADE_THIRDPARTY_ROOT "$ENV{OPENCASCADE_THIRDPARTY_ROOT}" CACHE PATH "OpenCASCADE 3rdparty root")
    else()
        set(OpenCASCADE_THIRDPARTY_ROOT
                "C:/Users/rdkgs/Downloads/occt-combined-with-debug-pch/opencascade-8.0.0-vc14-64-pch-with-debug-combined/3rdparty-vc14-64"
                CACHE PATH "OpenCASCADE 3rdparty root"
        )
    endif()

    set(OpenCASCADE_INCLUDE_DIR "${OpenCASCADE_ROOT}/inc")
    set(OpenCASCADE_LIBRARY_DIR "${OpenCASCADE_ROOT}/win64/vc14/lib")
    set(OpenCASCADE_BINARY_DIR  "${OpenCASCADE_ROOT}/win64/vc14/bin")
else()
    find_package(OpenCASCADE REQUIRED)
endif()

include(FetchContent)
FetchContent_Declare(
        svg
        GIT_REPOSITORY https://github.com/vincentlaucsb/svg.git
        GIT_TAG 0.5.0
)
FetchContent_Declare(
        CLI11
        GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
        GIT_TAG v2.6.2
)
FetchContent_MakeAvailable(svg CLI11)

add_executable(cadcam_hex_nut_m10
        src/main.cpp
        src/HexNutParams.hpp
)

target_include_directories(cadcam_hex_nut_m10 PRIVATE
        ${OpenCASCADE_INCLUDE_DIR}
)

if (WIN32)
    target_link_directories(cadcam_hex_nut_m10 PRIVATE
            ${OpenCASCADE_LIBRARY_DIR}
    )

    target_compile_definitions(cadcam_hex_nut_m10 PRIVATE
            NOMINMAX
            WIN32_LEAN_AND_MEAN
    )

    file(GLOB OPENCASCADE_DLLS "${OpenCASCADE_BINARY_DIR}/*.dll")
    file(GLOB_RECURSE THIRDPARTY_DLLS "${OpenCASCADE_THIRDPARTY_ROOT}/*.dll")

    set(ALL_DLLS ${OPENCASCADE_DLLS} ${THIRDPARTY_DLLS})
    list(LENGTH ALL_DLLS TOTAL_DLLS)
    set(DLL_INDEX 0)
    foreach(DLL ${ALL_DLLS})
        math(EXPR DLL_INDEX "${DLL_INDEX} + 1")
        add_custom_command(TARGET cadcam_hex_nut_m10 POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E echo
                "Copying DLL ${DLL_INDEX}/${TOTAL_DLLS}: ${DLL}"
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${DLL}"
                "$<TARGET_FILE_DIR:cadcam_hex_nut_m10>"
        )
    endforeach()
endif()

target_link_libraries(cadcam_hex_nut_m10 PRIVATE
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
        TKDESTEP
        TKDESTL
        TKOffset
        svg
        CLI11::CLI11
)
