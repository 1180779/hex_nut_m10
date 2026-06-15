cmake_minimum_required(VERSION 4.2)
project(cadcam_hex_nut_m10 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------
# Local configuration via .env (optional)
# ---------------------------------------------------------------------------
# Keep machine-specific paths out of this file. Create a ".env" next to this
# CMakeLists (it is git-ignored; see .env.example) with lines like:
#
#   OpenCASCADE_ROOT=C:/path/to/opencascade-8.0.0-vc14-64
#   OpenCASCADE_THIRDPARTY_ROOT=C:/path/to/3rdparty-vc14-64
#
# Precedence (highest first): -D<VAR> on the command line  >  real environment
# variable  >  .env  >  built-in defaults below.
set(_dotenv "${CMAKE_CURRENT_SOURCE_DIR}/.env")
if (EXISTS "${_dotenv}")
    message(STATUS "Loading local config from ${_dotenv}")
    file(STRINGS "${_dotenv}" _dotenv_lines)
    foreach (_line IN LISTS _dotenv_lines)
        string(STRIP "${_line}" _line)
        # Skip blank lines and comments (# or ;)
        if (_line STREQUAL "" OR _line MATCHES "^[#;]")
            continue()
        endif ()
        if (_line MATCHES "^([^=]+)=(.*)$")
            set(_key "${CMAKE_MATCH_1}")
            set(_val "${CMAKE_MATCH_2}")
            string(STRIP "${_key}" _key)
            string(STRIP "${_val}" _val)
            # Strip optional surrounding quotes
            string(REGEX REPLACE "^\"(.*)\"$" "\\1" _val "${_val}")
            string(REGEX REPLACE "^'(.*)'$" "\\1" _val "${_val}")
            # Only honour .env when neither -D nor a real env var already set it
            if (NOT DEFINED ${_key} AND NOT DEFINED ENV{${_key}})
                set(ENV{${_key}} "${_val}")
            endif ()
        endif ()
    endforeach ()
endif ()

# Promote selected environment variables (real or from .env) into cache vars
# so they can also be overridden with -D<VAR>=...
foreach (_var OpenCASCADE_ROOT OpenCASCADE_THIRDPARTY_ROOT)
    if (NOT DEFINED ${_var} AND DEFINED ENV{${_var}})
        set(${_var} "$ENV{${_var}}" CACHE PATH "Path provided via environment/.env")
    endif ()
endforeach ()

# ---------------------------------------------------------------------------
# Locate OpenCASCADE
# ---------------------------------------------------------------------------
# Two discovery modes:
#   * Windows binary distribution: point OpenCASCADE_ROOT at the unpacked
#     "opencascade-x.y.z-vcNN-64" folder. We derive inc/lib/bin from it and
#     glob the compiler subdir (vc14, vc143, ...) so any version works.
#   * Everything else (Linux/macOS, vcpkg, conda): use the package's own
#     CMake config. OpenCASCADE_ROOT, if set, is used as a search hint.
if (WIN32 AND OpenCASCADE_ROOT)
    set(OpenCASCADE_INCLUDE_DIR "${OpenCASCADE_ROOT}/inc")

    file(GLOB _occ_lib_dirs "${OpenCASCADE_ROOT}/win64/*/lib")
    file(GLOB _occ_bin_dirs "${OpenCASCADE_ROOT}/win64/*/bin")
    if (NOT _occ_lib_dirs)
        message(FATAL_ERROR
                "No lib directory found under '${OpenCASCADE_ROOT}/win64/*/lib'. "
                "Is OpenCASCADE_ROOT pointing at the correct distribution folder?")
    endif ()
    list(GET _occ_lib_dirs 0 OpenCASCADE_LIBRARY_DIR)
    if (_occ_bin_dirs)
        list(GET _occ_bin_dirs 0 OpenCASCADE_BINARY_DIR)
    endif ()
else ()
    find_package(OpenCASCADE REQUIRED)
    if (NOT OpenCASCADE_INCLUDE_DIR AND OpenCASCADE_INCLUDE_DIRS)
        set(OpenCASCADE_INCLUDE_DIR "${OpenCASCADE_INCLUDE_DIRS}")
    endif ()
    if (NOT OpenCASCADE_LIBRARY_DIR AND OpenCASCADE_LIBRARY_DIRS)
        set(OpenCASCADE_LIBRARY_DIR "${OpenCASCADE_LIBRARY_DIRS}")
    endif ()
endif ()

message(STATUS "OpenCASCADE include dir: ${OpenCASCADE_INCLUDE_DIR}")
message(STATUS "OpenCASCADE library dir: ${OpenCASCADE_LIBRARY_DIR}")

# Data-exchange toolkits were renamed in OCCT 7.7 (TKSTEP -> TKDESTEP, etc.).
# Resolve them by trying both spellings so a single config spans versions.
find_library(OPENCASCADE_STEP_TOOLKIT
        NAMES TKDESTEP TKSTEP
        HINTS "${OpenCASCADE_LIBRARY_DIR}" ${OpenCASCADE_LIBRARY_DIRS}
        REQUIRED
)
find_library(OPENCASCADE_STL_TOOLKIT
        NAMES TKDESTL TKSTL
        HINTS "${OpenCASCADE_LIBRARY_DIR}" ${OpenCASCADE_LIBRARY_DIRS}
        REQUIRED
)

message(STATUS "Using OpenCASCADE STEP toolkit: ${OPENCASCADE_STEP_TOOLKIT}")
message(STATUS "Using OpenCASCADE STL toolkit:  ${OPENCASCADE_STL_TOOLKIT}")

# ---------------------------------------------------------------------------
# Third-party dependencies
# ---------------------------------------------------------------------------
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

# ---------------------------------------------------------------------------
# Target
# ---------------------------------------------------------------------------
add_executable(cadcam_hex_nut_m10
        src/main.cpp
        src/HexNutParams.hpp
)

target_include_directories(cadcam_hex_nut_m10 PRIVATE
        ${OpenCASCADE_INCLUDE_DIR}
)

if (OpenCASCADE_LIBRARY_DIR)
    target_link_directories(cadcam_hex_nut_m10 PRIVATE ${OpenCASCADE_LIBRARY_DIR})
endif ()

if (WIN32)
    target_compile_definitions(cadcam_hex_nut_m10 PRIVATE
            NOMINMAX
            WIN32_LEAN_AND_MEAN
    )

    # Copy OCCT and 3rd-party DLLs next to the executable so it runs in-place.
    if (OpenCASCADE_BINARY_DIR)
        file(GLOB OPENCASCADE_DLLS "${OpenCASCADE_BINARY_DIR}/*.dll")
    endif ()
    if (OpenCASCADE_THIRDPARTY_ROOT)
        file(GLOB_RECURSE THIRDPARTY_DLLS "${OpenCASCADE_THIRDPARTY_ROOT}/*.dll")
    endif ()

    set(ALL_DLLS ${OPENCASCADE_DLLS} ${THIRDPARTY_DLLS})
    list(LENGTH ALL_DLLS TOTAL_DLLS)
    set(DLL_INDEX 0)
    foreach (DLL ${ALL_DLLS})
        math(EXPR DLL_INDEX "${DLL_INDEX} + 1")
        add_custom_command(TARGET cadcam_hex_nut_m10 POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E echo
                "Copying DLL ${DLL_INDEX}/${TOTAL_DLLS}: ${DLL}"
                COMMAND ${CMAKE_COMMAND}
                -D SOURCE_DLL="${DLL}"
                -D TARGET_DIR="$<TARGET_FILE_DIR:cadcam_hex_nut_m10>"
                -P "${CMAKE_CURRENT_SOURCE_DIR}/TryCopyDll.cmake"
        )
    endforeach ()
endif ()

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
        ${OPENCASCADE_STEP_TOOLKIT}
        ${OPENCASCADE_STL_TOOLKIT}
        TKOffset
        svg
        CLI11::CLI11
)
