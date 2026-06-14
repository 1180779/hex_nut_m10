#ifndef CAMCAM_HEX_NUT_M10_SVGEXPORTER_HPP
#define CAMCAM_HEX_NUT_M10_SVGEXPORTER_HPP

#include <svg.hpp>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <numbers>
#include <sstream>
#include <iomanip>

#include "HexNutParams.hpp"

/// @brief Exports the characteristic 2D curves of a hex nut to SVG.
/// Produces two ISO-style technical drawings:
///   - top view: hexagon profile (across corners e / across flats s), the
///     external-chamfer / bearing circle, and the conventional internal-thread
///     representation (solid minor crest and broken thin major circle);
///   - side view: a sectioned and hatched profile showing the external chamfers,
///     the threaded bore, and the countersinks, with the nut axis vertical
///
/// Both drawings carry symmetry axes (dash-dot center lines), dimensions in
/// millimetres, and a legend listing the parameter values
class SVGExporter {
public:
    explicit SVGExporter(const HexNutParams &params);

    void exportTopView(const std::string &filename);

    void exportSideView(const std::string &filename);

private:
    const HexNutParams &m_p;

    // drawing constants

    /// @brief px per mm
    static constexpr double SCALE = 18.0;

    /// @brief outer border
    static constexpr double MARGIN = 36.0;

    /// @brief room reserved for dimensions
    static constexpr double DIM_GAP = 64.0;

    /// @brief room reserved for the title
    static constexpr double TITLE_H = 26.0;

    /// @brief room reserved for the legend
    static constexpr double LEGEND_H = 78.0;
    static constexpr double FONT = 13.0;
    static constexpr double ARROW = 7.0;

    // colors

    static constexpr auto OUTLINE = "#1a1a1a";
    static constexpr auto THIN = "#1a1a1a";
    static constexpr auto AXIS = "#444444";
    static constexpr auto HATCH = "#888888";
    static constexpr auto DIMC = "#000000";

    // canvas origin of the current view (model 0,0 maps here); set per export

    double m_cx = 0.0;
    double m_cy = 0.0;

    /// @brief converts model X-coordinate to canvas X-coordinate
    /// @param mx model X-coordinate in millimeters
    /// @return canvas X-coordinate in pixels
    [[nodiscard]] double X(const double mx) const {
        return m_cx + mx * SCALE;
    }

    /// @brief converts model Y-coordinate to canvas Y-coordinate (flips axis)
    /// @param my model Y-coordinate in millimeters (Y-up)
    /// @return canvas Y-coordinate in pixels (Y-down)
    [[nodiscard]] double Y(const double my) const {
        return m_cy - my * SCALE;
    }

    /// @brief draws a line segment in canvas coordinates
    /// @param root SVG root object
    /// @param x1 starting X-coordinate
    /// @param y1 starting Y-coordinate
    /// @param x2 ending X-coordinate
    /// @param y2 ending Y-coordinate
    /// @param color stroke color string
    /// @param width stroke width in pixels
    /// @param dash optional stroke-dasharray string
    static void line(
        SVG::SVG &root,
        double x1,
        double y1,
        double x2,
        double y2,
        const std::string &color,
        double width,
        const std::string &dash = ""
    );

    /// @brief draws a circle in canvas coordinates
    /// @param root SVG root object
    /// @param cx center X-coordinate
    /// @param cy center Y-coordinate
    /// @param r radius in pixels
    /// @param color stroke color string
    /// @param width stroke width in pixels
    /// @param dash optional stroke-dasharray string
    static void circle(
        SVG::SVG &root,
        double cx,
        double cy,
        double r,
        const std::string &color,
        double width,
        const std::string &dash = ""
    );

    /// @brief draws an open polyline through canvas points
    /// @param root SVG root object
    /// @param pts sequence of points in canvas coordinates
    /// @param color stroke color string
    /// @param width stroke width in pixels
    /// @param dash optional stroke-dasharray string
    static void polyline(
        SVG::SVG &root,
        const std::vector<SVG::Point> &pts,
        const std::string &color,
        double width,
        const std::string &dash = ""
    );

    /// @brief draws a broken (gapped) circle
    /// @param root SVG root object
    /// @param cx center X-coordinate
    /// @param cy center Y-coordinate
    /// @param r radius in pixels
    /// @param color stroke color string
    /// @param width stroke width in pixels
    static void brokenCircle(
        SVG::SVG &root,
        double cx,
        double cy,
        double r,
        const std::string &color,
        double width
    );

    /// @brief draws a 45-degree section hatching clipped to a canvas rectangle
    /// @param root SVG root object
    /// @param x0 first corner X-coordinate
    /// @param x1 second corner X-coordinate
    /// @param y0 first corner Y-coordinate
    /// @param y1 second corner Y-coordinate
    static void hatchRect(SVG::SVG &root, double x0, double x1, double y0, double y1);

    /// @brief draws a filled triangular arrowhead at a given tip position and angle
    /// @param root SVG root object
    /// @param tipX X-coordinate of the tip
    /// @param tipY Y-coordinate of the tip
    /// @param angle orientation angle in radians (0 is right)
    static void arrowHead(SVG::SVG &root, double tipX, double tipY, double angle);

    /// @brief draws a text string on the canvas
    /// @param root SVG root object
    /// @param x X-coordinate
    /// @param y Y-coordinate
    /// @param s text content
    /// @param size font size in pixels
    /// @param anchor SVG text-anchor
    /// @param italic if true, uses italic font style
    /// @param rotateDeg optional rotation angle in degrees around (x, y)
    static void text(
        SVG::SVG &root,
        double x,
        double y,
        const std::string &s,
        double size,
        const std::string &anchor = "middle",
        bool italic = false,
        double rotateDeg = 0.0
    );

    /// @brief draws a horizontal dimension line with arrows and a label
    /// @param root SVG root object
    /// @param x1 starting X-coordinate of the dimension
    /// @param x2 ending X-coordinate of the dimension
    /// @param yDim Y-coordinate where the dimension line is drawn
    /// @param yFeature Y-coordinate of the feature being dimensioned (for witness lines)
    /// @param label text label to display above the line
    static void dimH(
        SVG::SVG &root,
        double x1,
        double x2,
        double yDim,
        double yFeature,
        const std::string &label
    );

    /// @brief draws a vertical dimension line with arrows and a label
    /// @param root SVG root object
    /// @param y1 starting Y-coordinate of the dimension
    /// @param y2 ending Y-coordinate of the dimension
    /// @param xDim X-coordinate where the dimension line is drawn
    /// @param xFeature X-coordinate of the feature being dimensioned (for witness lines)
    /// @param label text label to display beside the line
    static void dimV(
        SVG::SVG &root,
        double y1,
        double y2,
        double xDim,
        double xFeature,
        const std::string &label
    );

    /// @brief draws vertical and horizontal dash-dot symmetry axes
    /// @param root SVG root object
    /// @param halfW horizontal reach from the center in millimeters
    /// @param halfH vertical reach from the center in millimeters
    void centreLines(SVG::SVG &root, double halfW, double halfH) const;

    /// @brief draws a legend box with a title and parameter key-value pairs
    /// @param root SVG root object
    /// @param x top-left X-coordinate of the legend area
    /// @param y top-left Y-coordinate of the legend area
    /// @param title legend title text
    /// @param rows vector of parameter name and value string pairs
    static void legend(
        SVG::SVG &root,
        double x,
        double y,
        const std::string &title,
        const std::vector<std::pair<std::string, std::string>> &rows
    );

    static std::string num(double v);
};

inline SVGExporter::SVGExporter(const HexNutParams &params) : m_p(params) {}

inline void SVGExporter::line(
    SVG::SVG &root,
    const double x1,
    const double y1,
    const double x2,
    const double y2,
    const std::string &color,
    const double width,
    const std::string &dash
) {
    auto *l = root.add_child<SVG::Line>(
        SVG::Point{x1, y1},
        SVG::Point{x2, y2}
    );
    l->set_attr("stroke", color)
     .set_attr("stroke-width", width)
     .set_attr("stroke-linecap", "round");
    if (!dash.empty()) {
        l->set_attr("stroke-dasharray", dash);
    }
}

inline void SVGExporter::circle(
    SVG::SVG &root,
    const double cx,
    const double cy,
    const double r,
    const std::string &color,
    const double width,
    const std::string &dash
) {
    auto *c = root.add_child<SVG::Circle>(cx, cy, r);
    c->set_attr("fill", "none")
     .set_attr("stroke", color)
     .set_attr("stroke-width", width);
    if (!dash.empty()) {
        c->set_attr("stroke-dasharray", dash);
    }
}

inline void SVGExporter::polyline(
    SVG::SVG &root,
    const std::vector<SVG::Point> &pts,
    const std::string &color,
    const double width,
    const std::string &dash
) {
    if (pts.size() < 2) {
        return;
    }
    auto *p = root.add_child<SVG::Path>();
    p->start(pts.front().first, pts.front().second);
    for (std::size_t i = 1; i < pts.size(); ++i) {
        p->line_to(pts[i].first, pts[i].second);
    }
    p->set_attr("fill", "none")
     .set_attr("stroke", color)
     .set_attr("stroke-width", width)
     .set_attr("stroke-linejoin", "round");
    if (!dash.empty()) {
        p->set_attr("stroke-dasharray", dash);
    }
}

inline void SVGExporter::brokenCircle(
    SVG::SVG &root,
    const double cx,
    const double cy,
    const double r,
    const std::string &color,
    const double width
) {
    // 270-degree arc with the gap centred on the upper-right diagonal

    static constexpr double gap = std::numbers::pi_v<double> / 2.0;
    static constexpr double start = 0.0;
    static constexpr double sweep = 2.0 * std::numbers::pi_v<double> - gap;
    static constexpr int steps = 64;
    std::vector<SVG::Point> pts;
    pts.reserve(steps + 1);
    for (int i = 0; i <= steps; ++i) {
        const double a = start + sweep * static_cast<double>(i) / steps;
        pts.emplace_back(cx + r * std::cos(a), cy + r * std::sin(a));
    }
    polyline(root, pts, color, width);
}

inline void SVGExporter::hatchRect(SVG::SVG &root, double x0, double x1, double y0, double y1) {
    if (x0 > x1) {
        std::swap(x0, x1);
    }
    if (y0 > y1) {
        std::swap(y0, y1);
    }
    constexpr double spacing = 7.0;
    // 45-degree lines (slope -1 in Y-down SVG space, which is visually / upward-sloping)
    const double cStart = x0 + y0;
    const double cEnd = x1 + y1;
    const int numLines = static_cast<int>((cEnd - cStart) / spacing) + 1;
    for (int i = 0; i < numLines; ++i) {
        const double c = cStart + static_cast<double>(i) * spacing;
        // intersect line x + y = c with the rectangle box
        double xa = x0,
               ya = c - x0;
        double xb = x1,
               yb = c - x1;
        if (ya > y1) {
            ya = y1;
            xa = c - y1;
        }
        if (ya < y0) {
            ya = y0;
            xa = c - y0;
        }
        if (yb > y1) {
            yb = y1;
            xb = c - y1;
        }
        if (yb < y0) {
            yb = y0;
            xb = c - y0;
        }
        if (xa <= xb && xa >= x0 - 0.5 && xb <= x1 + 0.5) {
            line(root, xa, ya, xb, yb, HATCH, 0.5);
        }
    }
}

inline void SVGExporter::arrowHead(
    SVG::SVG &root,
    const double tipX,
    const double tipY,
    const double angle
) {
    constexpr double w = ARROW * 0.32;
    const double bx = tipX - ARROW * std::cos(angle);
    const double by = tipY - ARROW * std::sin(angle);
    const double nx = -std::sin(angle) * w;
    const double ny = std::cos(angle) * w;
    auto *poly = root.add_child<SVG::Polygon>(
        std::vector<SVG::Point>{
            {tipX, tipY},
            {bx + nx, by + ny},
            {bx - nx, by - ny}
        }
    );
    poly->set_attr("fill", DIMC)
        .set_attr("stroke", DIMC)
        .set_attr("stroke-width", 0.5);
}

inline void SVGExporter::text(
    SVG::SVG &root,
    const double x,
    const double y,
    const std::string &s,
    const double size,
    const std::string &anchor,
    const bool italic,
    const double rotateDeg
) {
    auto *t = root.add_child<SVG::Text>(x, y, s);
    t->set_attr("font-family", "Arial, Helvetica, sans-serif")
     .set_attr("font-size", size)
     .set_attr("text-anchor", anchor)
     .set_attr("fill", DIMC);
    if (italic) {
        t->set_attr("font-style", "italic");
    }
    if (rotateDeg != 0.0) {
        std::ostringstream tr;
        tr << "rotate(" << num(rotateDeg) << " " << num(x) << " " << num(y) << ")";
        t->set_attr("transform", tr.str());
    }
}

inline void SVGExporter::dimH(
    SVG::SVG &root,
    const double x1,
    const double x2,
    const double yDim,
    const double yFeature,
    const std::string &label
) {
    const double dir = (yDim >= yFeature)
                           ? 1.0
                           : -1.0;
    // extension lines
    line(root, x1, yFeature, x1, yDim + dir * 4.0, AXIS, 0.5);
    line(root, x2, yFeature, x2, yDim + dir * 4.0, AXIS, 0.5);

    // dimension line and outward arrow heads
    line(root, x1, yDim, x2, yDim, DIMC, 0.8);
    arrowHead(root, x1, yDim, std::numbers::pi_v<double>);
    arrowHead(root, x2, yDim, 0.0);

    text(root, (x1 + x2) / 2.0, yDim - 5.0, label, FONT, "middle", true);
}

inline void SVGExporter::dimV(
    SVG::SVG &root,
    const double y1,
    const double y2,
    const double xDim,
    const double xFeature,
    const std::string &label
) {
    const double dir = xDim >= xFeature
                           ? 1.0
                           : -1.0;
    // extension lines
    line(root, xFeature, y1, xDim + dir * 4.0, y1, AXIS, 0.5);
    line(root, xFeature, y2, xDim + dir * 4.0, y2, AXIS, 0.5);

    // dimension line and outward arrow heads
    line(root, xDim, y1, xDim, y2, DIMC, 0.8);
    arrowHead(root, xDim, y1, -std::numbers::pi_v<double> / 2.0);
    arrowHead(root, xDim, y2, std::numbers::pi_v<double> / 2.0);

    text(root, xDim - 5.0, (y1 + y2) / 2.0, label, FONT, "middle", true, -90.0);
}

inline void SVGExporter::centreLines(SVG::SVG &root, const double halfW, const double halfH) const {
    static constexpr auto dash = "10,3,2,3"; // dash-dot
    line(root, X(-halfW) - 8.0, Y(0.0), X(halfW) + 8.0, Y(0.0), AXIS, 0.6, dash);
    line(root, X(0.0), Y(halfH) - 8.0, X(0.0), Y(-halfH) + 8.0, AXIS, 0.6, dash);
}

inline void SVGExporter::legend(
    SVG::SVG &root,
    const double x,
    const double y,
    const std::string &title,
    const std::vector<std::pair<std::string, std::string>> &rows
) {
    text(root, x, y, title, FONT + 1.0, "start", false);
    text(root, x, y + 16.0, "Units: mm", FONT - 2.0, "start", false);
    double col = x;
    double row = y + 34.0;
    int i = 0;
    for (const auto &[dim, value] : rows) {
        std::string label = dim;
        label += " = ";
        label += value;
        text(root, col, row, label, FONT - 1.0, "start", true);
        row += 15.0;
        if (++i % 3 == 0) {
            col += 150.0;
            row = y + 34.0;
        }
    }
}

inline std::string SVGExporter::num(const double v) {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2) << v;
    return ss.str();
}

inline void SVGExporter::exportTopView(const std::string &filename) {
    const double e = m_p.getE();
    const double s = m_p.getS();
    const double rHex = s / std::sqrt(3.0);
    const double rDw = m_p.getDw() / 2.0;
    const double rBore = m_p.getD() / 2.0;
    const double rMajor = m_p.getD() / 2.0 + m_p.getThreadDepth();

    const double halfH = rHex;
    const double halfW = rHex * std::cos(std::numbers::pi_v<double> / 6.0);

    const double contentW = 2.0 * halfW * SCALE;
    const double contentH = 2.0 * halfH * SCALE;
    const double width = contentW + 2.0 * MARGIN + DIM_GAP;
    const double height = contentH + TITLE_H + 2.0 * MARGIN + DIM_GAP + LEGEND_H;

    m_cx = MARGIN + contentW / 2.0;
    m_cy = TITLE_H + MARGIN + contentH / 2.0;

    SVG::SVG root;
    root.set_attr("width", num(width))
        .set_attr("height", num(height))
        .set_attr("viewBox", "0 0 " + num(width) + " " + num(height));
    root.add_child<SVG::Rect>(0.0, 0.0, width, height)
        ->set_attr("fill", "white");

    text(
        root,
        width / 2.0,
        18.0,
        "Hex nut M" + num(m_p.getD()).substr(0, 2) + " -- Top view",
        FONT + 2.0,
        "middle"
    );

    centreLines(root, halfW, halfH);

    // hexagon: vertices at 30, 90, ..., 330 degrees
    std::vector<SVG::Point> hex;
    for (int i = 0; i < 6; ++i) {
        const double a = std::numbers::pi_v<double> / 6.0 +
            static_cast<double>(i) * std::numbers::pi_v<double> / 3.0;
        hex.emplace_back(X(rHex * std::cos(a)), Y(rHex * std::sin(a)));
    }
    hex.push_back(hex.front());
    polyline(root, hex, OUTLINE, 2.0);

    // bearing / external-chamfer circle, thread major (broken thin) and bore (solid)
    circle(root, X(0.0), Y(0.0), rDw * SCALE, THIN, 1.0);
    brokenCircle(root, X(0.0), Y(0.0), rMajor * SCALE, THIN, 1.0);
    circle(root, X(0.0), Y(0.0), rBore * SCALE, OUTLINE, 1.4);

    // dimensions: s across flats (bottom, horizontal), e across corners (right, vertical)
    const double yBot = Y(-halfH);
    dimH(root, X(-s / 2.0), X(s / 2.0), yBot + DIM_GAP * 0.55, yBot, "s = " + num(s));
    const double xRight = X(halfW);
    dimV(root, Y(halfH), Y(-halfH), xRight + DIM_GAP * 0.55, xRight, "e = " + num(e));

    legend(
        root,
        MARGIN,
        height - LEGEND_H + 4.0,
        "Top view",
        {
            {"s", num(s)},
            {"e", num(e)},
            {"D", num(m_p.getD())},
            {"dw", num(m_p.getDw())},
            {"P", num(m_p.getP())},
            {"da", num(m_p.getDa())}
        }
    );

    std::ofstream out(filename);
    out << std::string(root);
}

inline void SVGExporter::exportSideView(const std::string &filename) {
    // ISO orientation: the nut axis is horizontal. Model x is the axial
    // direction (nut height m), model y is the radial direction (across corners e).
    const double e = m_p.getE();
    const double s = m_p.getS();
    const double m = m_p.getM();
    const double beta = m_p.getBeta() * std::numbers::pi_v<double> / 180.0;
    const double ha = m / 2.0; // axial half-length
    const double he = e / 2.0; // radial half-width (corner)
    const double hdw = m_p.getDw() / 2.0; // bearing face half width
    const double rBore = m_p.getD() / 2.0; // minor crest (visible hole wall)
    const double rMajor = m_p.getD() / 2.0 + m_p.getThreadDepth(); // thread root
    const double rda = m_p.getDa() / 2.0; // countersink mouth

    // external chamfer depth (axial) at the corners, countersink depth (axial)
    const double cd = (he - hdw) * std::tan(beta);
    const double cdMax = std::max(cd, 0.0);
    const double csDepth = (rda - rBore) * std::tan(
        (180.0 - m_p.getTheta()) * std::numbers::pi_v<double> / 180.0
    );

    const double contentW = m * SCALE;
    const double contentH = e * SCALE;
    const double width = contentW + 2.0 * (MARGIN + DIM_GAP);
    const double height = contentH + TITLE_H + 2.0 * MARGIN + DIM_GAP + LEGEND_H;

    m_cx = MARGIN + DIM_GAP + contentW / 2.0;
    m_cy = TITLE_H + MARGIN + contentH / 2.0;

    SVG::SVG root;
    root.set_attr("width", num(width)).set_attr("height", num(height))
        .set_attr("viewBox", "0 0 " + num(width) + " " + num(height));
    root.add_child<SVG::Rect>(0.0, 0.0, width, height)->set_attr("fill", "white");

    text(
        root,
        width / 2.0,
        18.0,
        "Hex nut M" + num(m_p.getD()).substr(0, 2) +
        " - Side view (section)",
        FONT + 2.0,
        "middle"
    );

    // section hatching of the material bands,
    // clipped to the straight portion so it doesn't spill past the chamfers
    hatchRect(root, X(-(ha - cdMax)), X(ha - cdMax), Y(rBore), Y(he));
    hatchRect(root, X(-(ha - cdMax)), X(ha - cdMax), Y(-rBore), Y(-he));

    // outer profile with chamfered end corners
    auto outerHalf = [&](const double sign) {
        const std::vector<SVG::Point> p = {
            // right bearing edge
            {X(ha), Y(sign * hdw)},
            // chamfer to corner
            {X(ha - cd), Y(sign * he)},
            // straight edge (across corners)
            {X(-(ha - cd)), Y(sign * he)},
            // chamfer to left bearing edge
            {X(-ha), Y(sign * hdw)}
        };
        polyline(root, p, OUTLINE, 2.0);
    };
    outerHalf(1.0);
    outerHalf(-1.0);

    // left and right bearing faces
    line(root, X(ha), Y(-hdw), X(ha), Y(hdw), OUTLINE, 2.0);
    line(root, X(-ha), Y(-hdw), X(-ha), Y(hdw), OUTLINE, 2.0);

    // bore wall and countersinks
    auto boreHalf = [&](const double sign) {
        std::vector<SVG::Point> p = {
            // countersink mouth (right)
            {X(ha), Y(sign * rda)},
            // in to minor crest
            {X(ha - csDepth), Y(sign * rBore)},
            // straight hole wall
            {X(-(ha - csDepth)), Y(sign * rBore)},
            // countersink mouth (left)
            {X(-ha), Y(sign * rda)}
        };
        polyline(root, p, OUTLINE, 1.6);
    };
    boreHalf(1.0);
    boreHalf(-1.0);

    // simplified internal thread: thin major (root) lines along the bore
    line(root, X(-(ha - csDepth)), Y(rMajor), X(ha - csDepth), Y(rMajor), THIN, 0.8);
    line(root, X(-(ha - csDepth)), Y(-rMajor), X(ha - csDepth), Y(-rMajor), THIN, 0.8);

    centreLines(root, ha, he);

    // dimensions
    const double yBot = Y(-he);
    dimH(root, X(-ha), X(ha), yBot + DIM_GAP * 0.55, yBot, "m = " + num(m));

    const double xRight = X(ha);
    dimV(root, Y(he), Y(-he), xRight + DIM_GAP * 0.85, xRight, "e = " + num(e));
    dimV(root, Y(hdw), Y(-hdw), xRight + DIM_GAP * 0.4, xRight, "dw = " + num(m_p.getDw()));

    const double xLeft = X(-ha);
    dimV(root, Y(rBore), Y(-rBore), xLeft - DIM_GAP * 0.4, xLeft, "D = " + num(m_p.getD()));

    legend(
        root,
        MARGIN,
        height - LEGEND_H + 4.0,
        "Side view (section)",
        {
            {"m", num(m)},
            {"e", num(e)},
            {"s", num(s)},
            {"dw", num(m_p.getDw())},
            {"da", num(m_p.getDa())},
            {"D", num(m_p.getD())}
        }
    );

    std::ofstream out(filename);
    out << std::string(root);
}

#endif //CAMCAM_HEX_NUT_M10_SVGEXPORTER_HPP
