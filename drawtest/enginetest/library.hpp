#ifndef LIBRARY_HPP
#define LIBRARY_HPP

#include <cmath>
#include <iostream>
#include <iomanip>

constexpr float PI = 3.14159265358979323846f;


// 1. TEMPLATED 4D VECTOR

template <typename T = float>
struct VecT {
    T x{0};
    T y{0};
    T z{0};
    T w{1};

    // Operator Overloading: Vectoren optellen (v1 + v2)
    VecT operator+(const VecT& other) const {
        return {x + other.x, y + other.y, z + other.z, w + other.w};
    }

    // Operator Overloading: Vector vermenigvuldigen met getal (v * scalar)
    VecT operator*(T scalar) const {
        return {x * scalar, y * scalar, z * scalar, w * scalar};
    }

    // Operator Overloading: Direct printen met std::cout << v
    friend std::ostream& operator<<(std::ostream& os, const VecT& v) {
        return os << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
    }
};

// Default alias voor float-vectoren (zo blijft de rest van je engine werken)
using Vec = VecT<float>;

// 2. TEMPLATED 4x4 MATRIX
template <typename T = float>
struct MatrixT {
    T m[4][4]{
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    };

    // Operator Overloading: [] om rijen op te halen (bijv. mat[0][3])
    T* operator[](int index) { return m[index]; }
    const T* operator[](int index) const { return m[index]; }

    // Operator Overloading: Matrices optellen (m1 + m2)
    MatrixT operator+(const MatrixT& other) const {
        MatrixT res;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                res.m[i][j] = m[i][j] + other.m[i][j];
            }
        }
        return res;
    }

    // Operator Overloading: Direct printen met std::cout << matrix
    friend std::ostream& operator<<(std::ostream& os, const MatrixT& mat) {
        for (int i = 0; i < 4; ++i) {
            os << "[ ";
            for (int j = 0; j < 4; ++j) {
                os << std::setw(8) << std::setprecision(3) << mat.m[i][j] << " ";
            }
            os << "]\n";
        }
        return os;
    }
};

// Default alias voor float-matrices
using Matrix = MatrixT<float>;


// 3. TRANSFORM CLASS (Erft van Matrix<float>)

class Transform : public MatrixT<float> {
public:
    Transform() : MatrixT<float>() {}
    Transform(const MatrixT<float>& mat) : MatrixT<float>(mat) {}

    // Vermenigvuldiging Transform * Vector (inclusief perspectiefdeling)
    Vec operator*(const Vec& v) const {
        Vec res;
        res.x = m[0][0]*v.x + m[0][1]*v.y + m[0][2]*v.z + m[0][3]*v.w;
        res.y = m[1][0]*v.x + m[1][1]*v.y + m[1][2]*v.z + m[1][3]*v.w;
        res.z = m[2][0]*v.x + m[2][1]*v.y + m[2][2]*v.z + m[2][3]*v.w;
        res.w = m[3][0]*v.x + m[3][1]*v.y + m[3][2]*v.z + m[3][3]*v.w;

        if (std::abs(res.w) > 1e-6f) {
            res.x /= res.w;
            res.y /= res.w;
            res.z /= res.w;
            res.w = 1.0f;
        }
        return res;
    }

    // Vermenigvuldiging Transform * Transform
    Transform operator*(const MatrixT<float>& other) const {
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
};


// 4. SUBCLASSES (Scale, Rotate, Translate, Perspective)

enum class Axis { X, Y, Z };

class Scale : public Transform {
public:
    Scale(float s) : Scale(s, s, s) {}
    Scale(float sx, float sy, float sz) {
        m[0][0] = sx; m[1][1] = sy; m[2][2] = sz;
    }
};

class Rotate : public Transform {
public:
    Rotate(Axis axis, float degrees) {
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
};

class Translate : public Transform {
public:
    Translate(const Vec& t) : Translate(t.x, t.y, t.z) {}
    Translate(float tx, float ty, float tz) {
        m[0][3] = tx; m[1][3] = ty; m[2][3] = tz;
    }
};

class Perspective : public Transform {
public:
    Perspective(float fov_degrees) {
        float fov_rad = fov_degrees * (PI / 180.0f);
        float zp = 1.0f / std::tan(fov_rad / 2.0f);

        m[0][0] = zp;
        m[1][1] = zp;
        m[2][2] = zp;
        m[3][2] = 1.0f;
        m[3][3] = 0.0f;
    }
};

#endif // LIBRARY_HPP