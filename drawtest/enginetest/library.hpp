#ifndef LIBRARY_HPP
#define LIBRARY_HPP

#include <cmath>

inline constexpr float PI = 3.14159265359f;

struct Vec {
    float x{0.0f}, y{0.0f}, z{0.0f}, w{1.0f};
};

class Transform {
public:
    float m[4][4]; // Column-major: m[kolom][rij]

    inline Transform() {
        for (int c = 0; c < 4; ++c) {
            for (int r = 0; r < 4; ++r) {
                m[c][r] = (c == r) ? 1.0f : 0.0f;
            }
        }
    }
};

using Matrix4 = Transform;

class Translate : public Transform {
public:
    inline Translate(float tx, float ty, float tz) : Transform() {
        m[3][0] = tx;
        m[3][1] = ty;
        m[3][2] = tz;
    }
};

class Scale : public Transform {
public:
    inline Scale(float sx, float sy, float sz) : Transform() {
        m[0][0] = sx;
        m[1][1] = sy;
        m[2][2] = sz;
    }
};

class RotateY : public Transform {
public:
    inline RotateY(float angle_rad) : Transform() {
        float c = std::cos(angle_rad);
        float s = std::sin(angle_rad);
        m[0][0] = c;
        m[0][2] = -s;
        m[2][0] = s;
        m[2][2] = c;
    }
};

class RotateX : public Transform {
public:
    inline RotateX(float angle_rad) : Transform() {
        float c = std::cos(angle_rad);
        float s = std::sin(angle_rad);
        m[1][1] = c;
        m[1][2] = s;
        m[2][1] = -s;
        m[2][2] = c;
    }
};

class Perspective : public Transform {
public:
    inline Perspective(float fov_degrees, float aspect, float zNear = 0.1f, float zFar = 100.0f) : Transform() {
        float fov_rad = fov_degrees * (PI / 180.0f);
        float zp = 1.0f / std::tan(fov_rad / 2.0f);

        m[0][0] = zp / aspect;
        m[1][1] = zp;
        m[2][2] = (zFar + zNear) / (zNear - zFar);
        m[2][3] = -1.0f;
        m[3][2] = (2.0f * zFar * zNear) / (zNear - zFar);
        m[3][3] = 0.0f;
    }
};

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

#endif // LIBRARY_HPP