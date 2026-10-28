#ifndef LIBRARY_HPP
#define LIBRARY_HPP

#include <cmath>
#include <iostream>

inline constexpr float PI = 3.14159265359f;

// Basis Transform matrix (4x4 Identiteitsmatrix)
class Transform {
public:
    float m[4][4];

    inline Transform() {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                m[i][j] = (i == j) ? 1.0f : 0.0f;
            }
        }
    }
};

// Voeg deze aliases en de eenvoudige Vec struct toe aan library.hpp
struct Vec {
    float x{0.0f}, y{0.0f}, z{0.0f}, w{1.0f};
};

using Matrix4 = Transform;

// Translatie matrix
class Translate : public Transform {
public:
    inline Translate(float tx, float ty, float tz) : Transform() {
        m[0][3] = tx;
        m[1][3] = ty;
        m[2][3] = tz;
    }
};

// Schaal matrix
class Scale : public Transform {
public:
    inline Scale(float sx, float sy, float sz) : Transform() {
        m[0][0] = sx;
        m[1][1] = sy;
        m[2][2] = sz;
    }
};

// Rotatie om Y-as
class RotateY : public Transform {
public:
    inline RotateY(float angle_rad) : Transform() {
        m[0][0] = std::cos(angle_rad);
        m[0][2] = std::sin(angle_rad);
        m[2][0] = -std::sin(angle_rad);
        m[2][2] = std::cos(angle_rad);
    }
};

// Perspectief projectie matrix
class Perspective : public Transform {
public:
    inline Perspective(float fov_degrees, float aspect, float zNear = 0.1f, float zFar = 100.0f) : Transform() {
        float fov_rad = fov_degrees * (PI / 180.0f);
        float zp = 1.0f / std::tan(fov_rad / 2.0f);

        m[0][0] = zp / aspect;
        m[1][1] = zp;
        m[2][2] = (zFar + zNear) / (zNear - zFar);
        m[2][3] = (2.0f * zFar * zNear) / (zNear - zFar);
        m[3][2] = -1.0f;
        m[3][3] = 0.0f;
    }
};

// Matrix vermenigvuldiging (Transform * Transform)
inline Transform operator*(const Transform& a, const Transform& b) {
    Transform result;
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            result.m[r][c] = 0.0f;
            for (int k = 0; k < 4; ++k) {
                result.m[r][c] += a.m[r][k] * b.m[k][c];
            }
        }
    }
    return result;
}

#endif // LIBRARY_HPP