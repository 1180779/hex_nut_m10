#include <BRepPrimAPI_MakeBox.hxx>
#include <StlAPI_Writer.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <gp_Pnt.hxx>
#include <cmath>
#include <iostream>

#include "HexNutParams.hpp"

int main() {
    HexNutParams params{};

    const float R = params.getE() / 2.0f;
    constexpr int VERTEX_COUNT = 6;
    constexpr float ANGLE_STEP = 2.0f * static_cast<float>(M_PI) / VERTEX_COUNT;

    gp_Pnt hexVertices[VERTEX_COUNT];
    for (int i = 0; i < VERTEX_COUNT; ++i) {
        const float angle = static_cast<float>(i) * ANGLE_STEP;
        hexVertices[i] = gp_Pnt(R * std::cos(angle), 0.0f, R * std::sin(angle));
    }
    return 0;
}
