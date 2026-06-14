#include <BRepPrimAPI_MakeCylinder.hxx>
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
#include <BRepLib.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepOffsetAPI_MakePipeShell.hxx>
#include <STEPControl_Writer.hxx>
#include <ShapeFix_Shape.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS_Compound.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom2d_Line.hxx>
#include <gp_Pnt.hxx>
#include <gp_Circ.hxx>
#include <gp_Trsf.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp.hxx>
#include <stdexcept>
#include <string>
#include <iostream>
#include <numbers>

#include "HexNutParams.hpp"
#include "SVGExporter.hpp"

#define ASSERT_TRUE(expr) \
    do { \
        if (!(expr)) { \
            throw std::runtime_error(std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": " #expr " failed"); \
        } \
    } while (0)

#define ASSERT_DONE(builder) ASSERT_TRUE((builder).IsDone())

TopoDS_Face makeHexSketch(float circumradius);

TopoDS_Shape pad(const TopoDS_Face &sketch, float height);

TopoDS_Shape cutHole(const TopoDS_Shape &base, float radius, float height);

TopoDS_Shape makeChamferGrooveSketch(float dw, float e, float betaDeg, float m);

TopoDS_Shape makeInternalChamferGrooveSketch(float da, float m, float thetaDeg);

TopoDS_Shape makeThreadTool(float d, float m, float p, float threadDepth);

TopoDS_Shape trimExtensions(const TopoDS_Shape &shape, float trimR, float m);

TopoDS_Shape groove(const TopoDS_Shape &base, const TopoDS_Shape &sketch);

void checkShape(const TopoDS_Shape &shape, const char *tag);

class HexNutBuilder {
public:
    explicit HexNutBuilder(const HexNutParams &p) : m_p(p), m_extM(2.0f * p.getM()) {}

    HexNutBuilder& hexPad();

    HexNutBuilder& externalChamfer();

    HexNutBuilder& internalChamfer();

    HexNutBuilder& thread();

    HexNutBuilder& bore();

    HexNutBuilder& trim();

    [[nodiscard]] const TopoDS_Shape& shape() const {
        return m_shape;
    }

private:
    const HexNutParams &m_p;
    float m_extM;
    TopoDS_Shape m_shape;
};

int main() {
    const HexNutParams params{};

    // important: bore is cut after thread so the bore wall is never coincident with helix faces
    const TopoDS_Shape result = HexNutBuilder(params)
                                .hexPad()
                                .externalChamfer()
                                .internalChamfer()
                                .thread()
                                .bore()
                                .trim()
                                .shape();

    const BRepMesh_IncrementalMesh mesh(result, 0.01, false, 0.05);
    ASSERT_DONE(mesh);

    StlAPI_Writer hexWriter;
    hexWriter.Write(result, "hex.stl");

    STEPControl_Writer stepWriter;
    stepWriter.Transfer(result, STEPControl_AsIs);
    stepWriter.Write("hex.stp");

    SVGExporter svg(params);
    svg.exportTopView("hex_top.svg");
    svg.exportSideView("hex_side.svg");

    std::cout << "DONE" << std::endl;
    return 0;
}

TopoDS_Face makeHexSketch(const float circumradius) {
    static constexpr int VERTEX_COUNT = 6;
    static constexpr float ANGLE_STEP = 2.0f * std::numbers::pi_v<float> / VERTEX_COUNT;
    static constexpr auto ANGLE_OFFSET = std::numbers::pi_v<float> / 6.0f;

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
    t.SetTranslation(gp_Vec(gp::DZ()) * (-height / 2.0f));
    const TopoDS_Shape centered = BRepBuilderAPI_Transform(sketch, t).Shape();
    return BRepPrimAPI_MakePrism(centered, gp_Vec(gp::DZ()) * height);
}

TopoDS_Shape cutHole(const TopoDS_Shape &base, const float radius, const float height) {
    const TopoDS_Shape cyl = BRepPrimAPI_MakeCylinder(
        gp_Ax2(gp_Pnt(0, 0, -height / 2.0f), gp::DZ()),
        radius,
        height
    ).Shape();
    return BRepAlgoAPI_Cut(base, cyl).Shape();
}

TopoDS_Shape makeChamferGrooveSketch(const float dw, const float e, const float betaDeg, const float m) {
    const float dwd2 = dw / 2.0f;
    const float rOuter = e;
    const float md2 = m / 2.0f;
    const float betaRad = betaDeg * std::numbers::pi_v<float> / 180.0f;

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
    const float chamferAngleRad = (90.0f - thetaDeg / 2.0f) * std::numbers::pi_v<float> / 180.0f;

    // cone apex z-coordinate: where the slanted flank starting at (r = da / 2, z=+-m / 2)
    // intersects the revolution axis (r=0):
    // apexZ = md2 - dad2 * tan(90 - theta / 2)
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

TopoDS_Shape makeThreadTool(const float d, const float m, const float p, const float threadDepth) {
    const float rMinor = d / 2.0f - threadDepth;
    const float flat = p / 8.0f;
    const float flankH = threadDepth / std::sqrt(3.0f);

    // extend pi1/pi2 inward so the cut never touches the inner face
    static constexpr float EXT = 0.2f;
    const float flankNorm = std::sqrt(threadDepth * threadDepth + flankH * flankH);
    const float dr = EXT * threadDepth / flankNorm;
    const float dz = EXT * flankH / flankNorm;

    const float z0 = m / 2.0f;

    const gp_Pnt pi1(rMinor - dr, 0, z0 - dz);
    const gp_Pnt pi2(rMinor - dr, 0, z0 + 2.0f * flankH + flat + dz);
    const gp_Pnt po1(rMinor + threadDepth, 0, z0 + flankH);
    const gp_Pnt po2(rMinor + threadDepth, 0, z0 + flankH + flat);

    BRepBuilderAPI_MakeWire profWire;
    profWire.Add(BRepBuilderAPI_MakeEdge(pi1, pi2));
    profWire.Add(BRepBuilderAPI_MakeEdge(pi2, po2));
    profWire.Add(BRepBuilderAPI_MakeEdge(po2, po1));
    profWire.Add(BRepBuilderAPI_MakeEdge(po1, pi1));
    ASSERT_DONE(profWire);

    const float slope = p / (2.0f * std::numbers::pi_v<float>);
    const float norm2d = std::sqrt(1.0f + slope * slope);
    const float N = m / p + 2.0f;
    const float tEnd = 2.0f * std::numbers::pi_v<float> * N * norm2d;

    const gp_Ax3 cylAx(gp::Origin(), gp::DZ());
    Handle(Geom_CylindricalSurface) cyl = new Geom_CylindricalSurface(cylAx, rMinor);
    Handle(Geom2d_Line) helixLine =
        new Geom2d_Line(
            gp_Pnt2d(0.0f, z0),
            gp_Dir2d(-1.0f, -slope)
        );

    BRepBuilderAPI_MakeEdge edgeMaker(helixLine, cyl, 0.0, tEnd);
    ASSERT_DONE(edgeMaker);
    BRepLib::BuildCurve3d(edgeMaker.Edge());

    Standard_Real f, l;
    const Handle(Geom_Curve) helix3d = BRep_Tool::Curve(edgeMaker.Edge(), f, l);
    BRepBuilderAPI_MakeEdge spineEdge(helix3d, f, l);
    ASSERT_DONE(spineEdge);

    BRepBuilderAPI_MakeWire spineWire;
    spineWire.Add(spineEdge.Edge());
    ASSERT_DONE(spineWire);

    BRepOffsetAPI_MakePipeShell pipe(spineWire.Wire());
    pipe.SetMode(gp::DZ());
    pipe.Add(profWire.Wire());
    pipe.Build();
    ASSERT_DONE(pipe);
    ASSERT_TRUE(pipe.MakeSolid());

    ShapeFix_Shape fixer(pipe.Shape());
    fixer.Perform();
    return fixer.Shape();
}

TopoDS_Shape trimExtensions(const TopoDS_Shape &shape, const float trimR, const float m) {
    const TopoDS_Shape topTrim = BRepPrimAPI_MakeCylinder(
        gp_Ax2(
            gp_Pnt(0, 0, m / 2.0f),
            gp::DZ()
        ),
        trimR,
        m / 2.0f
    ).Shape();
    const TopoDS_Shape botTrim = BRepPrimAPI_MakeCylinder(
        gp_Ax2(
            gp_Pnt(0, 0, -m),
            gp::DZ()
        ),
        trimR,
        m / 2.0f
    ).Shape();
    const TopoDS_Shape result = BRepAlgoAPI_Cut(shape, topTrim).Shape();
    return BRepAlgoAPI_Cut(result, botTrim).Shape();
}

TopoDS_Shape groove(const TopoDS_Shape &base, const TopoDS_Shape &sketch) {
    const gp_Ax1 zAxis = gp::OZ();
    const TopoDS_Shape tool = BRepPrimAPI_MakeRevol(sketch, zAxis).Shape();
    return BRepAlgoAPI_Cut(base, tool).Shape();
}

void checkShape(const TopoDS_Shape &shape, const char *tag) {
    const BRepCheck_Analyzer ana(shape, true);
    if (ana.IsValid()) {
        std::cout << "[OK]   " << tag << "\n";
    }
    else {
        std::cout << "[FAIL] " << tag << " shape is invalid\n";
    }
}

HexNutBuilder& HexNutBuilder::hexPad() {
    const float circumradius = m_p.getS() / std::sqrt(3.0f);
    m_shape = pad(makeHexSketch(circumradius), m_extM);
    checkShape(m_shape, "hexPad");
    return *this;
}

HexNutBuilder& HexNutBuilder::externalChamfer() {
    m_shape = groove(
        m_shape,
        makeChamferGrooveSketch(m_p.getDw(), m_p.getE(), m_p.getBeta(), m_p.getM())
    );
    checkShape(m_shape, "externalChamfer");
    return *this;
}

HexNutBuilder& HexNutBuilder::internalChamfer() {
    m_shape = groove(
        m_shape,
        makeInternalChamferGrooveSketch(m_p.getDa(), m_p.getM(), m_p.getTheta())
    );
    checkShape(m_shape, "internalChamfer");
    return *this;
}

HexNutBuilder& HexNutBuilder::thread() {
    const TopoDS_Shape tool = makeThreadTool(
        m_p.getD(),
        m_extM,
        m_p.getP(),
        m_p.getThreadDepth()
    );
    BRepAlgoAPI_Cut cut(m_shape, tool);
    cut.Build();
    ASSERT_DONE(cut);
    m_shape = cut.Shape();
    checkShape(m_shape, "thread");
    return *this;
}

HexNutBuilder& HexNutBuilder::bore() {
    m_shape = cutHole(m_shape, m_p.getD() / 2.0f - m_p.getThreadDepth(), m_extM);
    checkShape(m_shape, "bore");
    return *this;
}

HexNutBuilder& HexNutBuilder::trim() {
    m_shape = trimExtensions(m_shape, m_p.getE(), m_p.getM());
    checkShape(m_shape, "trim");
    return *this;
}
