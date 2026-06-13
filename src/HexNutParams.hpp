//
// Created on 6/13/26.
//

#ifndef CAMCAM_HEX_NUT_M10_HEXNUTPARAMS_HPP
#define CAMCAM_HEX_NUT_M10_HEXNUTPARAMS_HPP

/// @brief Parameters for an ISO 4032 metric hex nut, including thread profile and bearing face dimensions
class HexNutParams {
private:
    /// @brief Basic major diameter (nominal diameter) of thread
    float m_D;

    /// @brief Pitch
    float m_P;

    /// @brief Diameter of the countersink
    float m_da;

    /// @brief Outer diameter of the bearing face
    float m_dw;

    /// @brief Width across corners
    float m_e;

    /// @brief Nut height
    float m_m;

    /// @brief Wrenching height
    float m_mw;

    /// @brief Width across flats
    float m_s;

    /// @brief Countersink angle
    float m_theta;

    /// @brief External chamfer angle
    float m_beta;

    /// @brief Thread flank angle
    float m_threadFlankAngle;

    /// @brief Fundamental triangle height
    float m_H;

    /// @brief Thread depth (nut)
    float m_threadDepth;

    /// @brief Crest truncation
    float m_crestTruncation;

    /// @brief Root truncation
    float m_rootTruncation;

public:
    // constants

    static constexpr float M10_NOMINAL_DIAMETER = 10.0f;
    static constexpr float FLOAT_COMP = 0.001f;

    /// @brief Constructs a HexNutParams initialised to M10 defaults per ISO 4032:2012
    HexNutParams()
        : m_D(0), m_P(0), m_da(0), m_dw(0), m_e(0), m_m(0), m_mw(0), m_s(0),
          m_theta(0), m_beta(0), m_threadFlankAngle(0), m_H(0),
          m_threadDepth(0), m_crestTruncation(0), m_rootTruncation(0)
    { setD(M10_NOMINAL_DIAMETER); }

    // getters

    /// @brief Returns the basic major diameter (nominal diameter) of the thread
    /// @return Basic major diameter in millimeters
    [[nodiscard]] float getD() const { return m_D; }

    /// @brief Returns the pitch (distance between adjacent thread crests)
    /// @return Pitch in millimeters
    [[nodiscard]] float getP() const { return m_P; }

    /// @brief Returns the diameter of the countersink
    /// @return Diameter of the countersink in millimeters
    [[nodiscard]] float getDa() const { return m_da; }

    /// @brief Returns the outer diameter of the bearing face
    /// @return Outer diameter of the bearing face in millimeters
    [[nodiscard]] float getDw() const { return m_dw; }

    /// @brief Returns the width across corners (circumscribed circle diameter)
    /// @return Width across corners in millimeters
    [[nodiscard]] float getE() const { return m_e; }

    /// @brief Returns the nut height
    /// @return Nut height in millimeters
    [[nodiscard]] float getM() const { return m_m; }

    /// @brief Returns the wrenching height
    /// @return Wrenching height in millimeters
    [[nodiscard]] float getMw() const { return m_mw; }

    /// @brief Returns the width across flats (inscribed circle diameter)
    /// @return Width across flats in millimeters
    [[nodiscard]] float getS() const { return m_s; }

    /// @brief Returns the countersink angle
    /// @return Countersink angle in degrees
    [[nodiscard]] float getTheta() const { return m_theta; }

    /// @brief Returns the external chamfer angle
    /// @return External chamfer angle in degrees
    [[nodiscard]] float getBeta() const { return m_beta; }

    /// @brief Returns the thread flank angle (half-angle between adjacent flanks)
    /// @return Thread flank angle in degrees
    [[nodiscard]] float getThreadFlankAngle() const { return m_threadFlankAngle; }

    /// @brief Returns the fundamental triangle height derived from pitch
    /// @return Fundamental triangle height in millimeters
    [[nodiscard]] float getH() const { return m_H; }

    /// @brief Returns the thread depth (nut engagement depth)
    /// @return Thread depth in millimeters
    [[nodiscard]] float getThreadDepth() const { return m_threadDepth; }

    /// @brief Returns the crest truncation (flat at thread tip)
    /// @return Crest truncation in millimeters
    [[nodiscard]] float getCrestTruncation() const { return m_crestTruncation; }

    /// @brief Returns the root truncation (flat at thread root)
    /// @return Root truncation in millimeters
    [[nodiscard]] float getRootTruncation() const { return m_rootTruncation; }

    /// @brief Sets the basic major diameter (nominal diameter) of the thread 
    /// and updates all related parameters based on ISO 4032:2012
    /// @param D Basic major diameter (nominal diameter) of thread in millimeters
    void setD(const float D) {
        m_D = D;

        // Lookup table based on ISO 4032:2012 for metric threads
        if (std::fabs(m_D - M10_NOMINAL_DIAMETER) < FLOAT_COMP) {
            m_P = 1.5f;
            m_da = 10.80f;
            m_dw = 14.60f;
            m_e = 17.77f;
            m_m = 8.40f;
            m_s = 16.00f;
        }
        // Add more entries as needed for other thread sizes

        // Calculate derived thread profile dimensions
        m_H = std::sqrt(3.0f) / 2.0f * m_P;
        m_threadDepth = 5.0f / 8.0f * m_H;
        m_crestTruncation = m_H / 8.0f;
        m_rootTruncation = m_H / 4.0f;

        // Set angular parameters
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

    /// @brief Sets the diameter of the countersink
    /// @param da Diameter of the countersink in millimeters
    void setDa(const float da) { m_da = da; }

    /// @brief Sets the outer diameter of the bearing face
    /// @param dw Outer diameter of the bearing face in millimeters
    void setDw(const float dw) { m_dw = dw; }

    /// @brief Sets the width across corners (circumscribed circle diameter)
    /// @param e Width across corners in millimeters
    void setE(const float e) { m_e = e; }

    /// @brief Sets the nut height
    /// @param m Nut height in millimeters
    void setM(const float m) { m_m = m; }

    /// @brief Sets the wrenching height
    /// @param mw Wrenching height in millimeters
    void setMw(const float mw) { m_mw = mw; }

    /// @brief Sets the width across flats (inscribed circle diameter)
    /// @param s Width across flats in millimeters
    void setS(const float s) { m_s = s; }

    /// @brief Sets the countersink angle
    /// @param theta Countersink angle in degrees
    void setTheta(const float theta) { m_theta = theta; }

    /// @brief Sets the external chamfer angle
    /// @param beta External chamfer angle in degrees
    void setBeta(const float beta) { m_beta = beta; }

    /// @brief Sets the thread flank angle (half-angle between adjacent flanks)
    /// @param angle Thread flank angle in degrees
    void setThreadFlankAngle(const float angle) { m_threadFlankAngle = angle; }
};

#endif //CAMCAM_HEX_NUT_M10_HEXNUTPARAMS_HPP
