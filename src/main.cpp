#include <BRepPrimAPI_MakeBox.hxx>
#include <StlAPI_Writer.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <cmath>
#include <stdexcept>
#include <string>

#define ASSERT_DONE(builder) \
    do { \
        if (!(builder).IsDone()) { \
            throw std::runtime_error(std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": " #builder " is not done"); \
        } \
    } while (0)

#include "HexNutParams.hpp"

TopoDS_Face makeHexSketch(const float circumradius) {
    constexpr int VERTEX_COUNT = 6;
    constexpr float ANGLE_STEP = 2.0f * static_cast<float>(M_PI) / VERTEX_COUNT;
    constexpr auto ANGLE_OFFSET = static_cast<float>(M_PI / 6.0);

    gp_Pnt vertices[VERTEX_COUNT];
    for (int i = 0; i < VERTEX_COUNT; ++i) {
        const float angle = static_cast<float>(i) * ANGLE_STEP + ANGLE_OFFSET;
        vertices[i] = gp_Pnt(circumradius * std::cos(angle), circumradius * std::sin(angle), 0.0f);
    }

    BRepBuilderAPI_MakeWire wireBuilder;
    for (int i = 0; i < VERTEX_COUNT; ++i)
        wireBuilder.Add(BRepBuilderAPI_MakeEdge(vertices[i], vertices[(i + 1) % VERTEX_COUNT]));

    ASSERT_DONE(wireBuilder);
    return BRepBuilderAPI_MakeFace(wireBuilder.Wire());
}

TopoDS_Shape pad(const TopoDS_Face &sketch, const float height) {
    gp_Trsf t;
    t.SetTranslation(gp_Vec(0, 0, -height / 2.0f));
    const TopoDS_Shape centered = BRepBuilderAPI_Transform(sketch, t).Shape();
    return BRepPrimAPI_MakePrism(centered, gp_Vec(0, 0, height));
}

int main() {
    const HexNutParams params{};

    const TopoDS_Face hexSketch = makeHexSketch(params.getE() / 2.0f);
    const TopoDS_Shape hexShape = pad(hexSketch, params.getM());

    const BRepMesh_IncrementalMesh mesh(hexShape, 0.1);
    ASSERT_DONE(mesh);

    StlAPI_Writer hexWriter;
    hexWriter.Write(hexShape, "hex.stl");
    std::cout << "DONE" << std::endl;
    return 0;
}
