//
// Created on 6/13/26.
//

#ifndef CAMCAM_HEX_NUT_M10_HEXNUTPARAMS_HPP
#define CAMCAM_HEX_NUT_M10_HEXNUTPARAMS_HPP

/// @brief parameters for an ISO 4032 metric hex nut, including thread profile and bearing face dimensions
class HexNutParams {
private:
    /// @brief basic major diameter (nominal diameter) of thread
    float m_D;

    /// @brief pitch
    float m_P;

    /// @brief diameter of the countersink
    float m_da;

    /// @brief outer diameter of the bearing face
    float m_dw;

    /// @brief width across corners
    float m_e;

    /// @brief nut height
    float m_m;

    /// @brief wrenching height
    float m_mw;

    /// @brief width across flats
    float m_s;

    /// @brief countersink angle
    float m_theta;

    /// @brief external chamfer angle
    float m_beta;

    /// @brief thread flank angle
    float m_threadFlankAngle;

    /// @brief fundamental triangle height
    float m_H;

    /// @brief thread depth (nut)
    float m_threadDepth;

    /// @brief crest truncation
    float m_crestTruncation;

    /// @brief root truncation
    float m_rootTruncation;

public:
    // constants

    static constexpr float M10_NOMINAL_DIAMETER = 10.0f;
    static constexpr float FLOAT_COMP = 0.001f;

    /// @brief constructs HexNutParams initialised to M10 defaults
    HexNutParams()
        : m_D(0), m_P(0), m_da(0), m_dw(0), m_e(0), m_m(0), m_mw(0), m_s(0),
          m_theta(0), m_beta(0), m_threadFlankAngle(0), m_H(0),
          m_threadDepth(0), m_crestTruncation(0), m_rootTruncation(0)
    { setD(M10_NOMINAL_DIAMETER); }

    // getters

    /// @brief returns the basic major diameter (nominal diameter) of the thread
    /// @return basic major diameter in millimeters
    [[nodiscard]] float getD() const { return m_D; }

    /// @brief returns the pitch (distance between adjacent thread crests)
    /// @return pitch in millimeters
    [[nodiscard]] float getP() const { return m_P; }

    /// @brief returns the diameter of the countersink
    /// @return diameter of the countersink in millimeters
    [[nodiscard]] float getDa() const { return m_da; }

    /// @brief returns the outer diameter of the bearing face
    /// @return outer diameter of the bearing face in millimeters
    [[nodiscard]] float getDw() const { return m_dw; }

    /// @brief returns the width across corners (circumscribed circle diameter)
    /// @return width across corners in millimeters
    [[nodiscard]] float getE() const { return m_e; }

    /// @brief returns the nut height
    /// @return nut height in millimeters
    [[nodiscard]] float getM() const { return m_m; }

    /// @brief returns the wrenching height
    /// @return wrenching height in millimeters
    [[nodiscard]] float getMw() const { return m_mw; }

    /// @brief returns the width across flats (inscribed circle diameter)
    /// @return width across flats in millimeters
    [[nodiscard]] float getS() const { return m_s; }

    /// @brief returns the countersink angle
    /// @return countersink angle in degrees
    [[nodiscard]] float getTheta() const { return m_theta; }

    /// @brief returns the external chamfer angle
    /// @return external chamfer angle in degrees
    [[nodiscard]] float getBeta() const { return m_beta; }

    /// @brief returns the thread flank angle (half-angle between adjacent flanks)
    /// @return thread flank angle in degrees
    [[nodiscard]] float getThreadFlankAngle() const { return m_threadFlankAngle; }

    /// @brief returns the fundamental triangle height derived from pitch
    /// @return fundamental triangle height in millimeters
    [[nodiscard]] float getH() const { return m_H; }

    /// @brief returns the thread depth
    /// @return thread depth in millimeters
    [[nodiscard]] float getThreadDepth() const { return m_threadDepth; }

    /// @brief returns the crest truncation (flat at thread tip)
    /// @return crest truncation in millimeters
    [[nodiscard]] float getCrestTruncation() const { return m_crestTruncation; }

    /// @brief returns the root truncation (flat at thread root)
    /// @return root truncation in millimeters
    [[nodiscard]] float getRootTruncation() const { return m_rootTruncation; }

    /// @brief sets the basic major diameter (nominal diameter) of the thread 
    /// and updates all related parameters based on ISO 4032:2012
    /// @param D basic major diameter (nominal diameter) of thread in millimeters
    void setD(const float D) {
        m_D = D;

        // lookup table for metric threads
        if (std::fabs(m_D - M10_NOMINAL_DIAMETER) < FLOAT_COMP) {
            m_P = 1.5f;
            m_da = 10.80f;
            m_dw = 14.60f;
            m_e = 17.77f;
            m_m = 8.40f;
            m_s = 16.00f;
        }
        // TODO: add more entries

        // calculate derived thread profile dimensions
        m_H = std::sqrt(3.0f) / 2.0f * m_P;
        m_threadDepth = 5.0f / 8.0f * m_H;
        m_crestTruncation = m_H / 8.0f;
        m_rootTruncation = m_H / 4.0f;

        // set angular parameters
        m_theta = 120.0f;
        m_beta = 30.0f;
        m_threadFlankAngle = 60.0f;
    }

    void setP(const float P) {
        m_P = P;
        // recalculate derived values
        m_H = std::sqrt(3.0f) / 2.0f * m_P;
        m_threadDepth = 5.0f / 8.0f * m_H;
        m_crestTruncation = m_H / 8.0f;
        m_rootTruncation = m_H / 4.0f;
    }

    /// @brief sets the diameter of the countersink
    /// @param da Diameter of the countersink in millimeters
    void setDa(const float da) { m_da = da; }

    /// @brief sets the outer diameter of the bearing face
    /// @param dw outer diameter of the bearing face in millimeters
    void setDw(const float dw) { m_dw = dw; }

    /// @brief sets the width across corners (circumscribed circle diameter)
    /// @param e width across corners in millimeters
    void setE(const float e) { m_e = e; }

    /// @brief sets the nut height
    /// @param m nut height in millimeters
    void setM(const float m) { m_m = m; }

    /// @brief sets the wrenching height
    /// @param mw wrenching height in millimeters
    void setMw(const float mw) { m_mw = mw; }

    /// @brief sets the width across flats (inscribed circle diameter)
    /// @param s width across flats in millimeters
    void setS(const float s) { m_s = s; }

    /// @brief sets the countersink angle
    /// @param theta countersink angle in degrees
    void setTheta(const float theta) { m_theta = theta; }

    /// @brief sets the external chamfer angle
    /// @param beta external chamfer angle in degrees
    void setBeta(const float beta) { m_beta = beta; }

    /// @brief sets the thread flank angle (half-angle between adjacent flanks)
    /// @param angle thread flank angle in degrees
    void setThreadFlankAngle(const float angle) { m_threadFlankAngle = angle; }
};

#endif //CAMCAM_HEX_NUT_M10_HEXNUTPARAMS_HPP
