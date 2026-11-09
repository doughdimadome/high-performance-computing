#ifndef LIBRARY_HPP
#define LIBRARY_HPP

#include <cmath>
#include <iostream>

// Pi voor omrekening van graden naar radialen
inline constexpr float PI = 3.14159265359f;


// VECTOR WIKUNDE & TEMPLATES
// Handleiding-eisen: Coördinaten-klassen, vectorbewerkingen en operator overloading

// Generic 3D Vector template voor flexibiliteit met types (float, int, double)
template <typename T = float>
struct Vec3T {
    T x{0}, y{0}, z{0};

    // Vector optellen en aftrekken
    inline Vec3T operator+(const Vec3T& v) const { return {x + v.x, y + v.y, z + v.z}; }
    inline Vec3T operator-(const Vec3T& v) const { return {x - v.x, y - v.y, z - v.z}; }
    inline Vec3T operator*(T s) const { return {x * s, y * s, z * s}; }
};

using Vec3 = Vec3T<float>;

// 4D Homogene coördinaten (w = 1.0 voor posities, w = 0.0 voor richtingsvectoren)
struct Vec4 {
    float x{0.0f}, y{0.0f}, z{0.0f}, w{1.0f};
};

// Lengte berekend met de 3D stelling van Pythagoras
inline float length(const Vec3& v) {
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

// Normaliseren maakt de lengte van een vector precies 1, zonder de richting aan te passen
inline Vec3 normalize(const Vec3& v) {
    float len = length(v);
    return (len > 1e-5f) ? Vec3{v.x / len, v.y / len, v.z / len} : Vec3{0.0f, 0.0f, 0.0f};
}

// Inproduct (Dot product): Geeft de cosinus van de hoek tussen twee vectoren.
// Wordt in de fragment shader gebruikt voor de diffuse en speculaire lichtberekening.
inline float dot(const Vec3& u, const Vec3& v) {
    return u.x * v.x + u.y * v.y + u.z * v.z;
}

// Uitproduct (Cross product): Berekent een vector die haaks op u en v staat.
// Handig om oppervlaktenormalen uit 3 hoekpunten te berekenen.
inline Vec3 cross(const Vec3& u, const Vec3& v) {
    return {
        u.y * v.z - u.z * v.y,
        u.z * v.x - u.x * v.z,
        u.x * v.y - u.y * v.x
    };
}


// MATRIX 4X4 (COLUMN-MAJOR LAYOUT)
// Matrix-klasse, transformaties en operator overloading

class Transform {
public:
    float m[4][4]; // Column-major: m[kolom][rij] voor directe aansluiting op OpenGL

    // Identiteitsmatrix (enen op de diagonaal)
    inline Transform() {
        for (int c = 0; c < 4; ++c) { 
            for (int r = 0; r < 4; ++r) { 
                m[c][r] = (c == r) ? 1.0f : 0.0f;
            }
        }
    }
};

using Matrix4 = Transform;

// Matrix vermenigvuldiging (A * B). Let op: Volgorde is bepalend (A*B != B*A)!
inline Transform operator*(const Transform& a, const Transform& b) {
    Transform result;
    for (int c = 0; c < 4; ++c) {
        for (int r = 0; r < 4; ++r) {
            result.m[c][r] = 0.0f;
            for (int k = 0; k < 4; ++k) {
                result.m[c][r] += a.m[k][r] * b.m[c][k];
            }
        }
    }
    return result;
}

// Vector-Matrix vermenigvuldiging voor coördinaten-transformaties
inline Vec4 operator*(const Transform& mat, const Vec4& v) {
    Vec4 res;
    res.x = mat.m[0][0]*v.x + mat.m[1][0]*v.y + mat.m[2][0]*v.z + mat.m[3][0]*v.w;
    res.y = mat.m[0][1]*v.x + mat.m[1][1]*v.y + mat.m[2][1]*v.z + mat.m[3][1]*v.w;
    res.z = mat.m[0][2]*v.x + mat.m[1][2]*v.y + mat.m[2][2]*v.z + mat.m[3][2]*v.w;
    res.w = mat.m[0][3]*v.x + mat.m[1][3]*v.y + mat.m[2][3]*v.z + mat.m[3][3]*v.w;
    return res;
}


// GAUSS-JORDAN ELIMINATIE & INVERSE MATRIX
// Oplossen van stelsels en matrix-inversie

inline bool inverse(const Transform& input, Transform& out_inverse) {
    float aug[4][8];

    // Vul de uitgebreide matrix [A | I]
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            aug[r][c] = input.m[c][r];
            aug[r][c + 4] = (r == c) ? 1.0f : 0.0f;
        }
    }

    // Gauss-Jordan eliminatie met pivoting
    for (int i = 0; i < 4; ++i) {
        int pivot = i;
        for (int r = i + 1; r < 4; ++r) {
            if (std::abs(aug[r][i]) > std::abs(aug[pivot][i])) pivot = r;
        }

        if (pivot != i) {
            for (int k = 0; k < 8; ++k) std::swap(aug[i][k], aug[pivot][k]);
        }

        // Als de pivot 0 is, is det(A) = 0. De matrix is dan niet inverteerbaar.
        if (std::abs(aug[i][i]) < 1e-6f) return false;

        float div = aug[i][i];
        for (int k = 0; k < 8; ++k) aug[i][k] /= div;

        for (int r = 0; r < 4; ++r) {
            if (r != i) {
                float factor = aug[r][i];
                for (int k = 0; k < 8; ++k) aug[r][k] -= factor * aug[i][k];
            }
        }
    }

    // Kopieer het resultaat A^-1
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            out_inverse.m[c][r] = aug[r][c + 4];
        }
    }
    return true;
}

// TRANSFORMATIE KLASSEN
// Translaties, rotaties (x,y,z) en schaling

class Translate : public Transform {
public:
    inline Translate(float tx, float ty, float tz) : Transform() {
        m[3][0] = tx; m[3][1] = ty; m[3][2] = tz;
    }
};

class Scale : public Transform {
public:
    inline Scale(float sx, float sy, float sz) : Transform() {
        m[0][0] = sx; m[1][1] = sy; m[2][2] = sz;
    }
};

class RotateX : public Transform {
public:
    inline RotateX(float a) : Transform() {
        float c = std::cos(a), s = std::sin(a);
        m[1][1] = c; m[1][2] = s; m[2][1] = -s; m[2][2] = c;
    }
};

class RotateY : public Transform {
public:
    inline RotateY(float a) : Transform() {
        float c = std::cos(a), s = std::sin(a);
        m[0][0] = c; m[0][2] = -s; m[2][0] = s; m[2][2] = c;
    }
};

class RotateZ : public Transform {
public:
    inline RotateZ(float a) : Transform() {
        float c = std::cos(a), s = std::sin(a);
        m[0][0] = c; m[0][1] = s; m[1][0] = -s; m[1][1] = c;
    }
};

// Perspectiefprojectie matrix (comprimeert de 3D-kijkpiramide naar Clip Space)
class Perspective : public Transform {
public:
    inline Perspective(float fov_deg, float aspect, float zNear = 0.1f, float zFar = 100.0f) : Transform() {
        float fov_rad = fov_deg * (PI / 180.0f);
        float zp = 1.0f / std::tan(fov_rad / 2.0f);
        m[0][0] = zp / aspect;
        m[1][1] = zp;
        m[2][2] = (zFar + zNear) / (zNear - zFar);
        m[2][3] = -1.0f;
        m[3][2] = (2.0f * zFar * zNear) / (zNear - zFar);
        m[3][3] = 0.0f;
    }
};

#endif // LIBRARY_HPP 