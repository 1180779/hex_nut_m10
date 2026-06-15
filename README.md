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
  - [OpenCASCADE](https://dev.opencascade.org/) (3D modelling kernel). On Linux/macOS
    (or via vcpkg/conda) it is located with `find_package`. For the Windows binary
    distribution, point CMake at the unpacked folder via `OpenCASCADE_ROOT` (see below).
  - [vincentlaucsb/svg](https://github.com/vincentlaucsb/svg) (header-only SVG writer, fetched via `FetchContent`).
  - [CLI11](https://github.com/CLIUtils/CLI11) (command-line parsing, fetched via `FetchContent`).

### Configuring OpenCASCADE paths (Windows)
Copy `.env.example` to `.env` and set `OpenCASCADE_ROOT` (and
`OpenCASCADE_THIRDPARTY_ROOT`) to your unpacked distribution folders. The `.env`
file is git-ignored so machine-specific paths stay out of version control.
Precedence (highest first): `-D<VAR>` on the cmake command line > real environment
variable > `.env` > defaults. The data-exchange toolkits are resolved by name, so
both pre- and post-7.7 OCCT releases (`TKSTEP`/`TKDESTEP`, `TKSTL`/`TKDESTL`) work.

## Getting Started
1. **Build**:
   ```bash
   mkdir build && cd build
   cmake ..
   make
   ```
2. **Run**:
   ```bash
   ./cadcam_hex_nut_m10
   ```
   With no arguments this generates `hex_M10_iso.stl`, `hex_M10_iso.stp`,
   `hex_M10_iso_top.svg`, and `hex_M10_iso_side.svg` in the current directory.
   Output paths can be overridden with `-o/--out-stl`, `--out-step`,
   `--out-svg-top`, and `--out-svg-side`.

3. **Parametric options** (run with `--help` for the full list):
   - `-d/--diameter` &mdash; nominal thread diameter (default 10), drives the ISO 4032 lookup.
   - `-p/--pitch`, `-s/--flats`, `-m/--height`, `--da`, `--dw` &mdash; override individual ISO table values.
   - `--theta` &mdash; internal countersink angle in degrees (default 120).
   - `--beta` &mdash; external chamfer angle in degrees (default 30).
   - `--flank-angle` &mdash; thread flank angle in degrees (default 60).

   When any geometry value is overridden, the auto-generated filenames encode the
   overrides (e.g. `hex_M10_theta90_side.svg`) instead of the `_iso` suffix.

## Technical Details
- **Coordinate System**: Model space uses mm (Y-up), exported to SVG canvas
  (Y-down) with a default scale of 18 pixels/mm.
- **Diameters**: `D` is the basic major (nominal) diameter; the internal thread
  runs from the minor diameter ($D_1 = D - 2 \cdot \tfrac{5}{8}H$) out to the major.
- **Thread Representation**: Internal threads follow conventional representation
  (solid line for minor diameter, thin broken arc for major diameter).
- **Section View**: The side view is a section cut across the corners ($e$) to
  align with the top view and accurately show chamfer extents.
