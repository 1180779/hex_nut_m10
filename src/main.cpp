#include <BRepPrimAPI_MakeBox.hxx>
#include <StlAPI_Writer.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS_Compound.hxx>
#include <gp_Pnt.hxx>
#include <gp_Circ.hxx>
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

TopoDS_Face makePocketSketch(const float radius) {
    const gp_Pnt center{};
    const gp_Dir unitZ{0.0f, 0.0f, 1.0f};
    const gp_Ax2 axis(center, unitZ);
    gp_Circ circle{axis, radius};

    BRepBuilderAPI_MakeEdge edgeMaker(circle);
    TopoDS_Edge edge = edgeMaker.Edge();

    BRepBuilderAPI_MakeWire wireBuilder;
    wireBuilder.Add(edge);
    ASSERT_DONE(wireBuilder);

    return BRepBuilderAPI_MakeFace(wireBuilder);
}

TopoDS_Shape pocket(const TopoDS_Shape &base, const TopoDS_Face &sketch, const float depth) {
    const TopoDS_Shape tool = pad(sketch, depth);
    return BRepAlgoAPI_Cut(base, tool).Shape();
}

TopoDS_Shape makeChamferGrooveSketch(const float dw, const float e, const float betaDeg, const float m) {
    const float dwd2 = dw / 2.0f;
    const float rOuter = e;
    const float md2 = m / 2.0f;
    const float betaRad = betaDeg * static_cast<float>(M_PI) / 180.0f;

    // p3 is the intersection of
    // the vertical line through p2 (y = rOuter)
    // and the line from p1 rising at angle beta above the horizontal p1-p2 baseline,
    // therefore,
    // p3.z = p2.z + (rOuter - dwd2) * tan(beta)
    const float chamferHeight = (rOuter - dwd2) * std::tan(betaRad);

    auto makeTri = [](const gp_Pnt &a, const gp_Pnt &b, const gp_Pnt &c) {
        BRepBuilderAPI_MakeWire w;
        w.Add(BRepBuilderAPI_MakeEdge(a, b));
        w.Add(BRepBuilderAPI_MakeEdge(b, c));
        w.Add(BRepBuilderAPI_MakeEdge(c, a));
        ASSERT_DONE(w);
        return BRepBuilderAPI_MakeFace(w.Wire()).Face();
    };

    // bottom chamfer triangle in YZ half-plane (x=0), revolved around Z
    const gp_Pnt botP1{0.0, dwd2, -md2};
    const gp_Pnt botP2{0.0f, rOuter, -md2};
    const gp_Pnt botP3{0.0f, rOuter, -md2 + chamferHeight};

    // top chamfer triangle: mirror of bottom about z=0 (i.e., the Y axis in the YZ sketch plane)
    const gp_Pnt topP1{0.0f, dwd2, md2};
    const gp_Pnt topP2{0.0f, rOuter, md2};
    const gp_Pnt topP3{0.0f, rOuter, md2 - chamferHeight};

    TopoDS_Compound compound;
    constexpr BRep_Builder builder;
    builder.MakeCompound(compound);
    builder.Add(compound, makeTri(botP1, botP2, botP3));
    builder.Add(compound, makeTri(topP1, topP2, topP3));
    return compound;
}

TopoDS_Shape makeInternalChamferGrooveSketch(const float da, const float m, const float thetaDeg) {
    const float dad2 = da / 2.0f;
    const float md2 = m / 2.0f;
    const float chamferAngleRad = (180.0f - thetaDeg) * static_cast<float>(M_PI) / 180.0f;

    // cone apex z-coordinate: where the slanted flank (at 180-theta from horizontal)
    // starting at (r = da / 2, z=+-m / 2) intersects the revolution axis (r=0):
    // apexZ = md2 - dad2 * tan(180 - theta)
    const float apexZ = md2 - dad2 * std::tan(chamferAngleRad);

    auto makeTri = [](const gp_Pnt &a, const gp_Pnt &b, const gp_Pnt &c) {
        BRepBuilderAPI_MakeWire w;
        w.Add(BRepBuilderAPI_MakeEdge(a, b));
        w.Add(BRepBuilderAPI_MakeEdge(b, c));
        w.Add(BRepBuilderAPI_MakeEdge(c, a));
        ASSERT_DONE(w);
        return BRepBuilderAPI_MakeFace(w.Wire()).Face();
    };

    // top countersink triangle in XZ half-plane (y=0), revolved around Z
    const gp_Pnt topP1{0.0f, 0.0f, md2};
    const gp_Pnt topP2{-dad2, 0.0f, md2};
    const gp_Pnt topP3{0.0f, 0.0f, apexZ};

    // bottom countersink: mirror of top about z=0 (i.e., the X axis in the XZ sketch plane)
    const gp_Pnt botP1{0.0f, 0.0f, -md2};
    const gp_Pnt botP2{dad2, 0.0f, -md2};
    const gp_Pnt botP3{0.0f, 0.0f, -apexZ};

    TopoDS_Compound compound;
    constexpr BRep_Builder builder;
    builder.MakeCompound(compound);
    builder.Add(compound, makeTri(topP1, topP2, topP3));
    builder.Add(compound, makeTri(botP1, botP2, botP3));
    return compound;
}

TopoDS_Shape groove(const TopoDS_Shape &base, const TopoDS_Shape &sketch) {
    const gp_Ax1 zAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
    const TopoDS_Shape tool = BRepPrimAPI_MakeRevol(sketch, zAxis).Shape();
    return BRepAlgoAPI_Cut(base, tool).Shape();
}

int main() {
    const HexNutParams params{};

    const TopoDS_Face hexSketch = makeHexSketch(params.getE() / 2.0f);
    const TopoDS_Shape hexShape = pad(hexSketch, params.getM());

    const TopoDS_Face pocketSketch = makePocketSketch(params.getD() / 2.0f);
    const TopoDS_Shape bored = pocket(hexShape, pocketSketch, params.getM());

    const TopoDS_Shape chamferSketch = makeChamferGrooveSketch(
        params.getDw(),
        params.getE(),
        params.getBeta(),
        params.getM()
    );
    const TopoDS_Shape withExternalChamfer = groove(bored, chamferSketch);

    const TopoDS_Shape internalChamferSketch = makeInternalChamferGrooveSketch(
        params.getDa(),
        params.getM(),
        params.getTheta()
    );
    const TopoDS_Shape result = groove(withExternalChamfer, internalChamferSketch);

    const BRepMesh_IncrementalMesh mesh(result, 0.1); //, false, 0.1);
    ASSERT_DONE(mesh);

    StlAPI_Writer hexWriter;
    hexWriter.Write(result, "hex.stl");
    std::cout << "DONE" << std::endl;
    return 0;
}
