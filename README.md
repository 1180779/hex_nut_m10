# Parametric ISO 4032 Hex Nut Generator

A university studies project exploring parametric CAD generation. It models an
ISO 4032 metric hex nut (default M10) in C++20, producing both 3D solid geometry
and 2D technical drawings from a single parametric definition.

## Features
- **ISO 4032:2012 Compliant**: Automatic lookup of standard dimensions
  ($P, d_a, d_w, e, m, m_w, s$) for the metric thread series (M1.6 &mdash; M20).
- **3D Solid Geometry**: Hex body, ISO 68-1 internal thread, external chamfers
  and internal countersink built with OpenCASCADE, exported to `hex.stl` and `hex.stp`.
- **Technical SVG Export**:
  - Orthographic views (top and side/section).
  - ISO-standard hatching and line styles (broken circles for thread roots).
  - Automated dimensioning and technical legend.
- **Parametric Design**: Driven by `HexNutParams`; easily extendable to other
  standard sizes or custom geometries.

## Project Structure
- `src/`: C++ source code (`main.cpp`, `HexNutParams.hpp`, `SVGExporter.hpp`).
- `docs/`: Technical documentation.
  - `LaTeX/`: Project reports, PDFs, and citations.
- `models/`: CAD source files, including the FreeCAD model (`hex_nut_M10.FCStd`).
- `cmake-build-debug/`: CMake build artifacts.

## Requirements
- **Compiler**: C++20 compatible (uses `<numbers>`, `std::to_array`, `std::ranges`).
- **Build System**: CMake 4.2+.
- **Dependencies**:
  - [OpenCASCADE](https://dev.opencascade.org/) (3D modelling kernel, found via `find_package`).
  - [vincentlaucsb/svg](https://github.com/vincentlaucsb/svg) (header-only SVG writer, fetched via `FetchContent`).

## Getting Started
1. **Build**:
   ```bash
   mkdir build && cd build
   cmake ..
   make
   ```
2. **Run**:
   ```bash
   ./camcam_hex_nut_m10
   ```
   This generates `hex.stl`, `hex.stp`, `hex_top.svg`, and `hex_side.svg` in the
   current directory.

## Technical Details
- **Coordinate System**: Model space uses mm (Y-up), exported to SVG canvas
  (Y-down) with a default scale of 18 pixels/mm.
- **Diameters**: `D` is the basic major (nominal) diameter; the internal thread
  runs from the minor diameter ($D_1 = D - 2 \cdot \tfrac{5}{8}H$) out to the major.
- **Thread Representation**: Internal threads follow conventional representation
  (solid line for minor diameter, thin broken arc for major diameter).
- **Section View**: The side view is a section cut across the corners ($e$) to
  align with the top view and accurately show chamfer extents.
