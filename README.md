# Parametric ISO 4032 Hex Nut Generator

A C++ utility for generating parametric ISO 4032 metric hex nuts. This tool provides both technical 2D documentation (SVG) and 3D geometry (planned Phase II).

## Features
- **ISO 4032:2012 Compliant**: Automatic lookup of standard dimensions ($P, d_a, d_w, e, m, s$) for metric threads.
- **Technical SVG Export**:
  - Orthographic views (Top and Side/Section).
  - ISO-standard hatching and line styles (broken circles for thread roots).
  - Automated dimensioning and technical legend.
- **Parametric Design**: Easily extendable to custom nut geometries or non-standard sizes.

## Project Structure
- `src/`: C++ source code (Header-only SVG library).
- `docs/`: Technical documentation.
  - `LaTeX/`: Project reports, PDFs, and citations.
- `models/`: CAD source files, including the FreeCAD model (`hex_nut_M10.FCStd`).
- `build/`: CMake build artifacts.

## Requirements
- **Compiler**: C++20 compatible (uses `<numbers>`, `std::numbers::pi`).
- **Build System**: CMake 3.15+.
- **Dependencies**: `vincentlaucsb/svg` (included via headers).

## Getting Started
1. **Build**:
   ```bash
   mkdir build && cd build
   cmake ..
   make
   ```
2. **Run**:
   ```bash
   ./camcam-hex-nut-m10
   ```
   This will generate `top_view.svg` and `side_view.svg` in the current directory.

## Technical Details
- **Coordinate System**: Model space uses mm (Y-up), exported to SVG canvas (Y-down) with a default scale of 18 pixels/mm.
- **Thread Representation**: Internal threads follow conventional representation (solid line for minor diameter, thin broken arc for major diameter).
- **Section View**: The side view is a section cut across the corners ($e$) to align with the top view and accurately show chamfer extents.
