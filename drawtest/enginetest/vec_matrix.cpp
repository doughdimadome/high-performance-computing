#include "library.hpp"
#include <iostream>
#include <iomanip>
#include <cmath>

constexpr float PI = 3.14159265358979323846f;

void print(const Vec &vec) {
    std::cout << "(" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ")\n";
}

void Matrix::print() const {
    for (int i = 0; i < 4; ++i) {
        std::cout << "[ ";
        for (int j = 0; j < 4; ++j) {
            std::cout << std::setw(8) << std::setprecision(3) << m[i][j] << " ";
        }
        std::cout << "]\n";
    }
}

// --- TRANSFORM IMPLEMENTATIE ---

Transform::Transform() : Matrix() {}

Transform::Transform(const Matrix& mat) : Matrix(mat) {}

Vec Transform::operator*(const Vec& v) const {
    Vec res;
    res.x = m[0][0]*v.x + m[0][1]*v.y + m[0][2]*v.z + m[0][3]*v.w;
    res.y = m[1][0]*v.x + m[1][1]*v.y + m[1][2]*v.z + m[1][3]*v.w;
    res.z = m[2][0]*v.x + m[2][1]*v.y + m[2][2]*v.z + m[2][3]*v.w;
    res.w = m[3][0]*v.x + m[3][1]*v.y + m[3][2]*v.z + m[3][3]*v.w;

    // Automatische perspectiefdeling zoals geëist in de opdracht
    if (std::abs(res.w) > 1e-6f) {
        res.x /= res.w;
        res.y /= res.w;
        res.z /= res.w;
        res.w = 1.0f;
    }
    return res;
}

Transform Transform::operator*(const Matrix& other) const {
    Transform res;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            res.m[i][j] = 0.0f;
            for (int k = 0; k < 4; ++k) {
                res.m[i][j] += this->m[i][k] * other.m[k][j];
            }
        }
    }
    return res;
}

// --- SUBCLASSES IMPLEMENTATIES ---

// Scale
Scale::Scale(float s) : Scale(s, s, s) {}

Scale::Scale(float sx, float sy, float sz) {
    m[0][0] = sx;
    m[1][1] = sy;
    m[2][2] = sz;
}

// Rotate
Rotate::Rotate(Axis axis, float degrees) {
    float rad = degrees * (PI / 180.0f);
    float c = std::cos(rad);
    float s = std::sin(rad);

    switch (axis) {
        case Axis::X:
            m[1][1] = c;  m[1][2] = -s;
            m[2][1] = s;  m[2][2] = c;
            break;
        case Axis::Y:
            m[0][0] = c;  m[0][2] = s;
            m[2][0] = -s; m[2][2] = c;
            break;
        case Axis::Z:
            m[0][0] = c;  m[0][1] = -s;
            m[1][0] = s;  m[1][1] = c;
            break;
    }
}

// Translate
Translate::Translate(const Vec& t) : Translate(t.x, t.y, t.z) {}

Translate::Translate(float tx, float ty, float tz) {
    m[0][3] = tx;
    m[1][3] = ty;
    m[2][3] = tz;
}

// Perspective
Perspective::Perspective(float fov_degrees) {
    float fov_rad = fov_degrees * (PI / 180.0f);
    float zp = 1.0f / std::tan(fov_rad / 2.0f);

    m[0][0] = zp;
    m[1][1] = zp;
    m[2][2] = zp;
    m[3][2] = 1.0f;
    m[3][3] = 0.0f;
}