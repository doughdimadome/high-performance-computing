#ifndef LIBRARY_HPP
#define LIBRARY_HPP

#include <cmath>
#include <utility> // Voor std::swap bij Gauss-Jordan pivoting

// Wiskundige constante Pi voor omrekening van graden naar radialen
inline constexpr float PI = 3.14159265359f;


// 1. VECTOR WISKUNDE (Templates, References & Operator Overloading)


// Generieke 3D-vector template (standaard float, maar werkt ook met int/double)
template <typename T = float>
struct Vec3T {
    T x{0}, y{0}, z{0};

    // Overloaded operators voor basis vectorbewerkingen
    constexpr Vec3T operator+(const Vec3T& v) const { return {x + v.x, y + v.y, z + v.z}; }
    constexpr Vec3T operator-(const Vec3T& v) const { return {x - v.x, y - v.y, z - v.z}; }
    constexpr Vec3T operator*(T scalar) const       { return {x * scalar, y * scalar, z * scalar}; }
};

// Type-alias zodat we overal kortweg 'Vec3' kunnen gebruiken
using Vec3 = Vec3T<float>;

// 4D Homogene vector (w = 1.0 voor een punt in de ruimte, w = 0.0 voor een richting)
struct Vec4 {
    float x{0.0f}, y{0.0f}, z{0.0f}, w{1.0f};
};

// Berekent de Euclidische lengte (norm) van een vector via Pythagoras in 3D
inline float length(const Vec3& v) {
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

// Normaliseert een vector naar lengte 1.0 (eenheidsvector) met beveiliging tegen delen door 0
inline Vec3 normalize(const Vec3& v) {
    float len = length(v);
    return (len > 1e-5f) ? Vec3{v.x / len, v.y / len, v.z / len} : Vec3{0.0f, 0.0f, 0.0f};
}

// Inproduct (Dot product): geeft ||u|| * ||v|| * cos(hoek)
// Essentieel voor lichtinval (diffuse & specular) in het Blinn-Phong model
constexpr float dot(const Vec3& u, const Vec3& v) {
    return u.x * v.x + u.y * v.y + u.z * v.z;
}

// Uitproduct (Cross product): levert een vector op die loodrecht (90 graden) op u en v staat
// Wordt gebruikt voor normaalvectoren en camera-assen
constexpr Vec3 cross(const Vec3& u, const Vec3& v) {
    return {
        u.y * v.z - u.z * v.y,
        u.z * v.x - u.x * v.z,
        u.x * v.y - u.y * v.x
    };
}


// 2. MATRIX 4X4 (Column-Major Layout voor OpenGL)


class Matrix4 {
public:
    // Opgeslagen als m[kolom][rij] (column-major), zodat OpenGL het geheugen direct kan uitlezen
    // Standaard geïnitialiseerd als de 4x4 Identiteitsmatrix (enen op de hoofddiagonaal)
    float m[4][4] = {
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    };

    // Matrix-matrix vermenigvuldiging (A * B)
    // Let op: niet commutatief (A * B != B * A); transformaties worden van rechts naar links toegepast
    constexpr Matrix4 operator*(const Matrix4& b) const {
        Matrix4 result{};
        for (int c = 0; c < 4; ++c) {
            for (int r = 0; r < 4; ++r) {
                result.m[c][r] = 0.0f;
                for (int k = 0; k < 4; ++k) {
                    result.m[c][r] += m[k][r] * b.m[c][k];
                }
            }
        }
        return result;
    }

    // Matrix-vector vermenigvuldiging (M * v) voor het transformeren van 4D-punten/vectoren
    constexpr Vec4 operator*(const Vec4& v) const;
};

// Definieer de operator buiten de klasse, nadat Vec4 volledig bekend is.
constexpr Vec4 Matrix4::operator*(const Vec4& v) const {
    return Vec4{
        m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0] * v.w,
        m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1] * v.w,
        m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2] * v.w,
        m[0][3] * v.x + m[1][3] * v.y + m[2][3] * v.z + m[3][3] * v.w
    };
}

// Alias voor achterwaartse compatibiliteit
using Transform = Matrix4;


// 3. GAUSS-JORDAN ELIMINATIE (Matrix Inversie)


// Berekent de inverse matrix A^-1 via Gauss-Jordan eliminatie met partial pivoting.
// Geeft 'false' terug als de determinant 0 is (niet-inverteerbare/singuliere matrix).
inline bool inverse(const Matrix4& input, Matrix4& out_inverse) {
    float aug[4][8];

    // Stap 1: Bouw de uitgebreide matrix [A | I] op (links invoer, rechts identiteitsmatrix)
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            aug[r][c]     = input.m[c][r];
            aug[r][c + 4] = (r == c) ? 1.0f : 0.0f;
        }
    }

    // Stap 2: Veeg elke kolom schoon naar gereduceerde rij-echelonvorm
    for (int i = 0; i < 4; ++i) {
        // Zoek de rij met het grootste spilelement (partial pivoting) voor numerieke stabiliteit
        int pivot = i;
        for (int r = i + 1; r < 4; ++r) {
            if (std::abs(aug[r][i]) > std::abs(aug[pivot][i])) {
                pivot = r;
            }
        }

        // Wissel de huidige rij met de pivot-rij
        if (pivot != i) {
            for (int k = 0; k < 8; ++k) {
                std::swap(aug[i][k], aug[pivot][k]);
            }
        }

        // Controleer of de matrix inverteerbaar is (spilelement mag niet 0 zijn)
        if (std::abs(aug[i][i]) < 1e-6f) return false;

        // Deel de pivot-rij zodat er een 1 op de hoofddiagonaal komt te staan
        float div = aug[i][i];
        for (int k = 0; k < 8; ++k) {
            aug[i][k] /= div;
        }

        // Maak alle overige getallen boven en onder de pivot in deze kolom 0
        for (int r = 0; r < 4; ++r) {
            if (r != i) {
                float factor = aug[r][i];
                for (int k = 0; k < 8; ++k) {
                    aug[r][k] -= factor * aug[i][k];
                }
            }
        }
    }

    // Stap 3: Kopieer de rechterhelft (A^-1) terug naar column-major formaat
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            out_inverse.m[c][r] = aug[r][c + 4];
        }
    }
    return true;
}


// 4. TRANSFORMATIE- EN PROJECTIEKLASSEN (Overerving van Matrix4)


// Translatie: plaatst de verschuiving (tx, ty, tz) in de 4e kolom van de matrix
class Translate : public Matrix4 {
public:
    constexpr Translate(float tx, float ty, float tz) {
        m[3][0] = tx;
        m[3][1] = ty;
        m[3][2] = tz;
    }
};

// Schaling: zet de schaalfactoren (sx, sy, sz) op de hoofddiagonaal
class Scale : public Matrix4 {
public:
    constexpr Scale(float sx, float sy, float sz) {
        m[0][0] = sx;
        m[1][1] = sy;
        m[2][2] = sz;
    }
};

// Rotatie om de X-as (hoek 'a' in radialen)
class RotateX : public Matrix4 {
public:
    RotateX(float a) {
        float c = std::cos(a), s = std::sin(a);
        m[1][1] = c;  m[1][2] = s;
        m[2][1] = -s; m[2][2] = c;
    }
};

// Rotatie om de Y-as (hoek 'a' in radialen)
class RotateY : public Matrix4 {
public:
    RotateY(float a) {
        float c = std::cos(a), s = std::sin(a);
        m[0][0] = c; m[0][2] = -s;
        m[2][0] = s; m[2][2] = c;
    }
};

// Rotatie om de Z-as (hoek 'a' in radialen)
class RotateZ : public Matrix4 {
public:
    RotateZ(float a) {
        float c = std::cos(a), s = std::sin(a);
        m[0][0] = c;  m[0][1] = s;
        m[1][0] = -s; m[1][1] = c;
    }
};

// Perspectiefprojectie: vormt het kijkfrustum om naar Normalized Device Coordinates (NDC)
class Perspective : public Matrix4 {
public:
    Perspective(float fov_deg, float aspect, float zNear = 0.1f, float zFar = 100.0f) {
        float fov_rad = fov_deg * (PI / 180.0f);
        float focalLength = 1.0f / std::tan(fov_rad / 2.0f);

        m[0][0] = focalLength / aspect;
        m[1][1] = focalLength;
        m[2][2] = (zFar + zNear) / (zNear - zFar);
        m[2][3] = -1.0f; // Kopieert -z naar de w-component voor perspectiefdeling
        m[3][2] = (2.0f * zFar * zNear) / (zNear - zFar);
        m[3][3] = 0.0f;
    }
};

#endif // LIBRARY_HPP